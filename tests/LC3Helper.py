def SETDR(x):
 return (((x) & 0x7) << 9)
def SETSR1(x): 
  return (((x) & 0x7) << 6)
def SETBASER(x):
  return (((x) & 0x7) << 6)
def SETSR2(x):
  return (((x) & 0x7) << 0)
def SETN(x):
  return (((x) & 0x1) << 11)
def SETZ(x): 
  return (((x) & 0x1) << 10)
def SETP(x): 
  return (((x) & 0x1) << 9)
def neg(x):
  return x & 0xFFFF
  
STEERINGNORM = 1<<5
ADDR = 0x1000
ADDI = ADDR | STEERINGNORM
ANDR = 0x5000
ANDI = ANDR | STEERINGNORM
BR   = 0x0000
NOP  = 0x0000
JMP  = 0xC000
JSR  = 0x4800
JSRR = 0x4000
LD   = 0x2000
LDI  = 0xA000
LDR  = 0x6000
LEA  = 0xE000
NOT  = 0x903F
RET  = JMP | SETSR1(7)
RTI  = 0x8000
ST   = 0x3000
STI  = 0xB000
STR  = 0x7000
TRAP = 0xF000
RESERVEDINST = 0xD000
