#include "simulator.hpp"
#include "simulator-internals.hpp"
#include <boost/python.hpp>
#include <fstream>
#include <iostream>

simulator::simulator( ){
        memory = vector<uint16_t>(ADDRESS_SPACE);
        if (!(this->loadBinFile(PREFIX "lc3os.obj"))) {
                std::cerr << "WARNING: could not load lc3 os."
                          << " expected to find it at "
                          << PREFIX  << "lc3os.obj" << "\n"
                          << "  Functions like puts and gets will not work.\n" ;
        }
        memory[0xFE04] = 0x8000;
}

/**
 * @brief step forward N cycles
 * @details  step forward N cycles
 * 
 * @param cycles number of cycles to step
 * @return false if exception occurred
 */
bool simulator::stepN( int cycles){
        return this->simulate(cycles, true, false);
}

bool simulator::nextN( int cycles){
        return this->simulate(cycles, false, false);
}

bool callsFunctionP( uint16_t instruction) {
        enum opcode op = inst2opcode(instruction);
        return (((op == JSR)) || (op == TRAP));
}

bool returnsP( uint16_t instruction) {
        enum opcode op = inst2opcode(instruction);
        return ((op == RTI) || ((op == JMP) && (inst2sr1(instruction) == 7)));
}


bool simulator::simulate( int cycles, bool countCallsP, bool stopOnRetP) {
        int cyclesElapsed = 0;
        bool exceptionP = true;
        bool breakPointTriggered = false;

        uint16_t lastinst = 0;
        int callCount = 0, retCount = 0;

        while (exceptionP && ((cycles == 0) || (cyclesElapsed < cycles))){
                if(prevBreakPtState == WAS_BREAKPOINT){
                    prevBreakPtState = NOT_BREAKPOINT;
                }else{
                    for (std::vector<BreakPoint>::iterator it
                                 = this->breakPoints.begin()
                                 ; it != this->breakPoints.end()
                                 ; ++it){
                            if (it->address == this->PC) {
                                    breakPointTriggered = true; //Lets 
                                    if (it->PythonP)
                                            boost::python::call<void>(it->Pycb
                                                                      , it->address);
                                    else if (it->Ccb)
                                            it->Ccb(it->address);
                            }
                    }
                }
                if(breakPointTriggered){
                    prevBreakPtState = WAS_BREAKPOINT;
                    return false;
                }

                lastinst = this->memory[this->PC];
                exceptionP = this->doInst(this->memRead(this->PC));
                if (returnsP(lastinst)) retCount++;
                else if (callsFunctionP(lastinst))callCount++;
                if (retCount > callCount) retCount = callCount = 0;
                if (countCallsP || retCount == callCount) cyclesElapsed++;

                for (std::vector<InterruptTrigger>::iterator it
                             = this->interruptTriggers.begin()
                             ; it != this->interruptTriggers.end()
                             ; ++it){
                        if (!it->triggeredP && boost::python::call<bool>(it->cb)) {
                                it->triggeredP = true;
                        }
                        if (it->triggeredP && it->priority > this->Priority) {
                                it->triggeredP = false;
                                if (this->getPcsrBit('s')) {
                                        this->USP = this->getReg(6);
                                        this->setReg(6,this->SSP);
                                }
                                this->setReg(6, this->getReg(6) - 1);
                                this->memWrite(this->getReg(6)
                                               , this->getPcsrBit('s') << 15
                                               | this->Priority << 8
                                               | this->getPcsrBit('n') << 2
                                               | this->getPcsrBit('z') << 1
                                               | this->getPcsrBit('p') << 0);
                                this->setReg(6, this->getReg(6) - 1);
                                this->memWrite(this->getReg(6),this->PC);
                                this->setPcsrBit('s',false);
                                this->PC = this->memRead(it->address + 0x100);
                        }
                }
        }

        //GUI Hook
        if(onEndOfCycle){
            onEndOfCycle();
        }

        return exceptionP;
}

/**
 * @brief Load a LC3 Object file
 * @details LC3 object files are binary files where the
 * first word denotes the starting address of the program
 * 
 * @param filename path to file
 * @return successful?
 */
bool simulator::loadBinFile( std::string filename ) {
        int fileSize = 0;
        uint16_t startAddr = 0;
        uint16_t data = 0;
        std::ifstream in;
        in.open(filename);
        in.seekg(0, in.end);
        fileSize = in.tellg();
        in.seekg(0, in.beg);
        if (fileSize < 2) return false;
        in.read((char*)&startAddr, 2);
        startAddr ^= ((startAddr & 0xFF) << 8);
        startAddr ^= startAddr >> 8;
        startAddr ^= ((startAddr & 0xFF) << 8);
        if (startAddr + fileSize - 2 > (1<<16)) return false;
        setPC(startAddr);
        while (! in.eof()) {
                in.read((char*)&data,2);
                data ^= ((data & 0xFF) << 8);
                data ^= data >> 8;
                data ^= ((data & 0xFF) << 8);

                //Update the GUI
                if(onMemChanged){
                    onMemChanged(startAddr, data);
                }
                this->memory[startAddr++] = data;

        }
        in.close();
        isHalted = false;
        return true;
}

/**
 * @brief call watchpoint callbacks
 * @details Watchpoints have python callbacks, e.g. a print function
 * or some other process. Equivelent to onChanged(mem[Address]) do callback
 *
 * @param WatchPoint Triggered watch point
 */
void callCallback (struct WatchPoint toCall) {
        if (toCall.PythonP)
                boost::python::call<void>(toCall.Pycb
                                          , toCall.address
                                          , toCall.prevVal
                                          , toCall.currVal);
        else if (toCall.Ccb)
                toCall.Ccb(toCall.address
                              , toCall.prevVal
                              , toCall.currVal);
}

/**
 * @brief Read a memory location
 * @details reads from memory. Triggers watch points if necessary
 * 
 * @param addr Address to read from
 * @return the data at mem[addr]
 */
uint16_t simulator::memRead( uint16_t addr ) {
        for (std::vector<WatchPoint>::iterator it = this->watchPoints.begin()
                     ; it != this->watchPoints.end()
                     ; ++it){
                if (it->readPoint && (it->address == addr)) {
                        callCallback(*it);
                }
        }
        return this->memory[addr];
}

/**
 * @brief Write to a memory location
 * @details writes to memory. Triggers watch points if necessary
 * 
 * @param addr Memory address to access
 * @param newVal mem[addr] = newVal
 * @return none
 */
void simulator::memWrite( uint16_t addr, uint16_t newVal ) {
        this->memory[addr] = newVal;
        for (std::vector<WatchPoint>::iterator it = this->watchPoints.begin()
                     ; it != this->watchPoints.end()
                     ; ++it){
                if (it->writePoint && (it->address == addr)) {
                        it->currVal = newVal;
                        callCallback(*it);
                        it->prevVal = newVal;
                }
        }
        if(onMemChanged){
            onMemChanged(addr, newVal);
        }
}

/**
 * @brief Manually set the NZP bits of PSR
 * @details Manually set the NZP bits of PSR
 * 
 * @param result None
 */
void simulator::setNZP( uint16_t result ) {
        int16_t signedResult = (int16_t) result;
        this->N = false;
        this->Z = false;
        this->P = false;
        if (signedResult < 0) this->N = true;
        else if (signedResult == 0) this-> Z = true;
        else if (signedResult > 0) this->P = true;
}

/**
 * @brief Process an instruction
 * @details This is equivelent to decode execute
 * 
 * @param inst 16 bit instruction to process
 * @return exception occurred?
 */
bool simulator::doInst( uint16_t inst ) {
        uint16_t result = 0;
        //LC3 PreIncrements the PC
        this->PC = (this->PC + 1) % ADDRESS_SPACE;

        switch (inst2opcode(inst)) {
        case ADD:
                if (inst2steering(inst)) {
                        result = this->regs[inst2sr1(inst)]
                                + inst2imm5(inst);
                } else {
                        result = this->regs[inst2sr1(inst)]
                                + this->regs[inst2sr2(inst)];
                }
                break;

        case AND:
                if (inst2steering(inst)) {
                        result = this->regs[inst2sr1(inst)]
                                & inst2imm5(inst);
                } else {
                        result = this->regs[inst2sr1(inst)]
                                & this->regs[inst2sr2(inst)];
                }
                break;

        case NOT:
                result = ~this->regs[inst2sr1(inst)];
                break;
        case LD:
        case LDI:
        case ST:
        case STI:
        case LEA:
                result = this->PC + inst2imm9(inst);
                break;

        case LDR:
        case STR:
                result = this->regs[inst2sr1(inst)] + inst2imm6(inst);
                break;

        case BR:
                if ((this->N && inst2n(inst))
                    || (this->Z && inst2z(inst))
                    || (this->P && inst2p(inst)))
                        this->PC += inst2imm9(inst);
                break;

        case JSR:
                this->regs[7] = this->PC;
        case JMP:
                if (inst2n(inst))
                        this->PC += inst2imm11(inst);
                else
                        this->PC = this->regs[inst2sr1(inst)];
                break;

        case TRAP:
                if(inst2trapvec8(inst) == 0x25) {
                        std::cerr << "found the halt instruction\n";
                        this->isHalted = true;
                        return false;
                }
                this->regs[7] = this->PC;
                this->PC = this->memRead(inst2trapvec8(inst));
                break;

        case RTI:
                if (this->S) return false;
                PC = this->memRead(this->regs[6]);
                this->regs[6]++;
                result = this->memRead(this->regs[6]);
                this->S = (result & (1 << 15)) != 0;
                this->Priority = (result & (0x7 << 8)) >> 8;
                this->N = (result & (1 << 2))  != 0;
                this->Z = (result & (1 << 1))  != 0;
                this->P = (result & (1 << 0))  != 0;
                this->regs[6]++;
                if (this->S == 1) {
                        this->SSP = this->getReg(6);
                        this->setReg(6,this->USP);
                }
                break;

        default:
                return false;
        }

        switch (inst2opcode(inst)){
        case LDI:
                result = this->memRead(result);
        case LD:
        case LDR:
                result = this->memRead(result);
        case ADD:
        case AND:
        case NOT:
        case LEA:
                this->regs[inst2dr(inst)] = result;
                this->setNZP(result);
                break;
        case JSR:
        case JMP:
        case BR:
        case TRAP:
        case RTI:
                break;
        case STI:
                result = this->memRead(result);
        case ST:
        case STR:
                this->memWrite(result,this->regs[inst2dr(inst)]);
                break;
        default:
                return false;
        }
        return true;
}

/**
 * @brief Get a status bit from the PSR
 * @details request a status bit from the PSR using its character
 * 
 * @param mnemonic n, z, p, or s
 * @return the status bit
 */
bool simulator::getPcsrBit( char mnemonic ) {
        switch (mnemonic) {
        case 'n':
        case 'N':
                return this->N == 1;
        case 'z':
        case 'Z':
                return this->Z == 1;
        case 'p':
        case 'P':
                return this->P == 1;
        case 's':
        case 'S':
                return this->S == 1;
        default:
                return false;

        }
}

/**
 * @brief Set status bit in PSR
 * @details Set a specific status bit in the PSR
 * 
 * @param mnemonic character of bit to set
 * @param newVal true/false
 * 
 * @return successful?
 */
bool simulator::setPcsrBit( char mnemonic , bool newVal) {
        switch (mnemonic) {
        case 'n':
        case 'N':
                this->N = newVal;
                break;
        case 'z':
        case 'Z':
                this->Z = newVal;
                break;
        case 'p':
        case 'P':
                this->P = newVal;
                break;
        default:
                return false;

        }
        //GUI Hook
        if(onEndOfCycle){
            onEndOfCycle();
        }
        return true;

}

/**
 * @brief Read from the register file
 * @details Read from the register file
 * 
 * @param number Register Number
 * @return register data
 */
uint16_t simulator::getReg( int number ) {
        return (number > NUM_REGS) ? 0 : this->regs[number];
}

/**
 * @brief Write to the register file
 * @details Write to the register file
 * 
 * @param number Register Number
 * @param newVal reg <= newVal
 * 
 * @return successful?
 */
bool simulator::setReg( int number, uint16_t newVal ) {
        if (number > NUM_REGS) {
                return false;
        } else {
                this->regs[number] = newVal;
               //GUI Hook
                if(onEndOfCycle){
                    onEndOfCycle();
                }
                return true;
        }
}

/**
 * @brief Get a slice of memory
 * @details get a slice of memory to work with
 * 
 * @param start starting address
 * @param stop end address
 * 
 * @return a copy of a slice of memory
 */
vector<uint16_t> simulator::sliceMem( uint16_t start, uint16_t stop ){
        return vector<uint16_t>(this->memory.begin() + start,
                                this->memory.begin() + stop );
}

/**
 * @brief Set the Current PC
 * @details Checks if the desired PC is Valid, then sets it
 * @return boolean if valid or not
 */
uint16_t simulator::getPC(void){ return PC; }
bool simulator::setPC(uint16_t pc){
        if(pc >= ADDRESS_SPACE) return false;
        this->PC = pc;
        //GUI Hook
        if(onEndOfCycle){
            onEndOfCycle();
        }
        return true;
}

uint16_t simulator::getPriority(void){ return this->Priority; }
bool simulator::setPriority(uint8_t newPriority){
        if(Priority >= 8) return false;
        this->Priority = newPriority;
        return true;
}

/**
 * @brief Add a watch point to the simulator
 * @details Watch points can be triggered on any combination of read/write
 * to a specific address. On triggering, the watchpoints call a python callback
 * function to handle the event. These call backs could be as simple as a print statement.
 * It also accepts python lambdas.
 * 
 * @param addr Address of memory to watch
 * @param read trigger on mem read
 * @param write trigger on mem write
 * @param cb call back function on event triggered
 * @return successful?
 */
bool simulator::addWatchPointPy(uint16_t addr, bool read, bool write, PyObject* cb){
        if(addr >= 1 << 16) return false;
        WatchPoint wp;
        wp.PythonP = true;
        wp.Pycb = cb;
        wp.address = addr;
        wp.readPoint = read;
        wp.writePoint = write;
        wp.prevVal = memory[addr];
        wp.currVal = memory[addr];
        watchPoints.push_back(wp);
        return true;
}

bool simulator::addWatchPoint(uint16_t addr, bool read, bool write
                              , std::function<void (uint16_t, uint16_t, uint16_t)>
                              cb){
        if(addr >= 1 << 16) return false;
        WatchPoint wp;
        wp.PythonP = false;
        wp.Ccb = cb;
        wp.address = addr;
        wp.readPoint = read;
        wp.writePoint = write;
        wp.prevVal = memory[addr];
        wp.currVal = memory[addr];
        watchPoints.push_back(wp);
        return true;
}

/**
 * @brief Helper function returns number of watch points
 * @return number of watchpoints
 */
int simulator::getNumWatchPoints(){
        return this->watchPoints.size();
}

bool simulator::addBreakPointPy(uint16_t addr, PyObject* cb) {
        if(addr >= 1<<16) return false;
        BreakPoint bp;
        bp.PythonP = true;
        bp.Pycb = cb;
        bp.address = addr;
        breakPoints.push_back(bp);
        return true;
}

bool simulator::addBreakPoint(uint16_t addr, std::function<void (uint16_t)> cb) {
        if(addr >= 1<<16) return false;
        BreakPoint bp;
        bp.PythonP = false;
        bp.Ccb = cb;
        bp.address = addr;
        breakPoints.push_back(bp);
        return true;
}

bool simulator::run(){
        return this->stepN(0);
}

void doJack(uint16_t address, uint16_t newVal){
    return;
}
void simulator::setOnMemChanged(std::function<void (uint16_t, uint16_t)> handlerFunction){
    onMemChanged = handlerFunction;
}
void simulator::setOnEndOfCycle(std::function<void (void)> handlerFunction){
    onEndOfCycle = handlerFunction;
}
void simulator::setRefreshGUIMemHook(std::function<void (void)> handlerFunction){
    refreshGUIMem = handlerFunction;
}
void simulator::refreshGUIMemCall(void){
    if(refreshGUIMem) refreshGUIMem();
}

bool simulator::addInterruptTrigger(uint8_t intnum, uint8_t priority, PyObject* cb) {
        if(intnum < 1<<7) return false;
        InterruptTrigger it;
        it.cb = cb;
        it.address = intnum;
        it.priority = priority;
        it.triggeredP = false;
        interruptTriggers.push_back(it);
        return true;
}

void simulator::removeBreakPoint(uint16_t addr){
    for (std::vector<BreakPoint>::iterator it
             = this->breakPoints.begin()
             ; it != this->breakPoints.end()
             ; ++it){
        if (it->address == addr) {
            breakPoints.erase(it);
            break;    
        }
    }
}
