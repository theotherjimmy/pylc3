#pragma once

#include <boost/python.hpp>
#include "simulator.hpp"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace py = boost::python;
using MEM_type = vector<uint16_t>;


BOOST_PYTHON_MODULE(pylc3)
{
  using namespace boost::python;
  class_<MEM_type>("memory")
    .def(vector_indexing_suite<MEM_type>());

  /*
    The simulator object can be created in python with:
    ```
    import pylc3
    sim = pylc3.simulator()
    ```
    The following comments assume that this bit of code
    has been run in your python interperator or script
    file already.
   */
  object pySim = class_<simulator>("simulator")
          /*
            stepN steps the simulated LC3 processor by n instructions
            including instructions taken by interrupt service
            routines and calls to functions. For example running
            the LC3 for 10 instructions starting at PC = 0x3000
            would go like:
            ```
            sim.setPC(0x3000)
            sim.stepN(10)
            ```
           */
    .def("stepN", &simulator::stepN)
          /*
            nextN steps the simulated LC3 processor by n instructions
            excluding instructions taken by interrupt service
            routines and calls to functions. For example running
            the LC3 for 15 instructions starting at PC = 0x4000
            would go like:
            ```
            sim.setPC(0x4000)
            sim.nextN(15)
            ```
           */
    .def("nextN", &simulator::nextN)
          /*
            +internal-ish+
            doInst does the instruction proveded as the first parameter.
            for example, a no-op:
            ```
            sim.doInst(0x0000)
            ```
          */
    .def("doInst"  , &simulator::doInst)
          /*
            Memory is provided as an array in the mem attribute of all
            simulator objects, and can be sliced like a python array.
            for example reading the contents of memory location 0x1000,
            and setting it to 4:
            ```
            oldval = sim.mem[0x1000]
            sim.mem[0x1000] = 4
            ```
           */
    .def_readwrite("mem", &simulator::memory)
          /*
            getReg and setReg get and set general purpose register in
            the simulated LC3. e.g.:
            ```
            oldReg = sim.getReg(5)
            sim.setReg(0,oldReg)
            ```
           */
    .def("getReg"  , &simulator::getReg)
    .def("setReg"  , &simulator::setReg)
          /*
            getPcsrBit and setPcsrBit get and set the bits of the LC3's
            program status register. The availible bits at this time are:
            s - supervisor mode
            n - negative condition code
            z - zero condition code
            p - positive condition code
            NOTE: it is possible to put the processor into an invalid
            state with the use of this function; for example you can set
            all three of n, z, and p.
            As an example use of this API, the code below sets the n bit
            and clears the z bit if the z bit is set:
            ```
            if sim.getPcsrBit('z') :
                sim.setPcsrBit('z', False)
                sim.setPcsrBit('n', True)
            ```
           */
    .def("getPcsrBit", &simulator::getPcsrBit)
    .def("setPcsrBit", &simulator::setPcsrBit)
          /*
            addWatchPoint adds a watch point to the simulator. Watchpoints
            in this LC3 simulator are setup for batch testing or
            programming and call a function when the program accesses a
            particular memory location, including for fetching instructions.
            This function acts as if it had been declared as follows in
            python:
            ```
            class simulator:
                ...
                def addWatchPoint(address, readP, writeP, callback):
                    ...
            ```
            The readP and writeP boolean values that tell the simulator
            to call the callback on a read or a write respectively. Both
            readP and writeP may be set. For example, we run the function
            `watch1` when the program writes to the memory location 0x4000
            and the function `watch2` when the program reads from the
            memory location 0x5000:
            ```
            def watch1(address, oldval, newval) :
                print("wachpoint 1: {:x}: {} -> {}".format(adderss
                                                           , oldval
                                                           , newval))
            def watch2(address, oldval, newval) :
                print("wachpoint 2: {:x}: {} -> {}".format(adderss
                                                           , oldval
                                                           , newval))
            sim.addWatchPoint(0x4000, False, True, watch1)
            sim.addWatchPoint(0x5000, True, False, watch2)
            ```
           */
    .def("addWatchPoint", &simulator::addWatchPointPy)
          /*
            addBreakPoint adds a watch point to the simulator. Breakpoints
            in this LC3 simulator are setup for batch testing or
            programming and call a function when the program fetches a
            particular memory location. For example, if we wanted to run
            the function `break1` when the program reaches 0x3010:
            ```
            def break1(address) :
                print("program reached {}", address)
            sim.addBreakPoint(0x3010, break1)
            ```
           */
    .def("addBreakPoint", &simulator::addBreakPointPy)
          /*
            load a program in the LC3 object file format from disk and
            load it into memory. Does not cause watchpoints to trigger.
            e.g. loading the program `iggy`:
            ```
            sim.load("iggy")
            ```
           */
    .def("load", &simulator::loadBinFile)
          /*
           */
    .def("run", &simulator::run)
          /*
            Call this function when using the gui after modifying memory
            for example:
            ```
            sim.mem[0x3000:0x4000] = range(0x1000)
            sim.refreshGUI()
            ```
           */
    .def("refreshGUI", &simulator::refreshGUIMemCall)
          /*
            getPC or setPC get or set the simulated LC3's progam counter
            example usage:
            ```
            oldpc = sim.getPC()
            sim.setPC(0x3000)
            ```
           */
    .def("getPC", &simulator::getPC)
    .def("setPC", &simulator::setPC)
          /*
            Interrupts in this LC3 simulator are generated by callbacks
            that execute every instruction and return a boolean indicating
            wheather either an interrupt should be triggered or not. Once
            an interrupt is triggered, the function triggering the interrupt
            will not be called again until the interrupt has been serviced
            by the processor.
            Example: suppose we want to trigger interrupt 0x85 with the
            python function `interrupt1` with the priority 4:
            ```
            counter = 0
            def interrupt1 ():
                counrter += 1
                if counter == 400:
                    counter = 0
                    return True
                return False
            sim.addInterruptTrigger(0x85, 4, interrupt1)
            ```
           */
    .def("addInterruptTrigger", &simulator::addInterruptTrigger)
          /*
            getPriority and setPriority get and set the interal priority
            that the simulated LC3 is running at.
            for example to set the proirity to 0:
            ```
            oldpriority = sim.getPriority()
            sim.setPriority(0)
            ```
           */
    .def("getPriority", &simulator::getPriority)
    .def("setPriority", &simulator::setPriority);

    object mSim = pySim(); //Construct one
}
