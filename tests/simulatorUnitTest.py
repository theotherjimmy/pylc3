#!/usr/bin/env python2.7

import pylc3
import unittest
from LC3Helper import *
from grader import doTest
false = False
true = True

class TestAdd(unittest.TestCase):
  #setUp gets called once before each test
  def setUp(self):
    self.sim = pylc3.simulator()
    self.sim.setReg(0, 2);
    self.sim.setReg(1, 2);
    self.sim.setReg(2, neg(-4));

  #All tests must have 'test_' followed by the test name
  def test_ADDR_1(self):
    self.sim.doInst(ADDR | SETDR(3) | SETSR1(0) | SETSR2(1))
    self.assertEqual(4,     self.sim.getReg(3))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(True,  self.sim.getPcsrBit('p'))

  def test_ADDR_2(self):
    self.sim.doInst(ADDR | SETDR(4) | SETSR1(0) | SETSR2(2)) 
    self.assertEqual(neg(-2), self.sim.getReg(4))
    self.assertEqual(True, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(False, self.sim.getPcsrBit('p'))

  def test_ADDR_3(self):
    self.sim.doInst(ADDR | SETDR(3) | SETSR1(0) | SETSR2(1))

    self.sim.doInst(ADDR | SETDR(3) | SETSR1(3) | SETSR2(2))
    self.assertEqual(0, self.sim.getReg(3))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(True, self.sim.getPcsrBit('z'))
    self.assertEqual(False, self.sim.getPcsrBit('p'))
 
  def test_ADDI(self):
    self.sim.setReg(0, 2)
    self.sim.doInst(ADDI | SETDR(3) | SETSR1(0) | (2 & 0x1F))
    self.assertEqual(4, self.sim.getReg(3))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(True, self.sim.getPcsrBit('p'))
 
    self.sim.doInst(ADDI | SETDR(4) | SETSR1(0) | ((neg(-4) & 0x1F)))
    self.assertEqual(neg(-2), self.sim.getReg(4))
    self.assertEqual(True, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(False, self.sim.getPcsrBit('p'))
 
    self.sim.doInst(ADDI | SETDR(3) | SETSR1(3) | ((neg(-4) & 0x1F)))
    self.assertEqual(0, self.sim.getReg(3))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(True, self.sim.getPcsrBit('z'))
    self.assertEqual(False, self.sim.getPcsrBit('p'))

class TestAnd(unittest.TestCase):
  def setUp(self):
    #Create a simulator
    self.sim = pylc3.simulator()

    self.sim.setReg(0, 0x1010)
    self.sim.setReg(1, 0xFFFF)
    self.sim.setReg(2, 0x1000)
    self.sim.setReg(3, 0x0000)

  def test_AND1(self):
    self.sim.doInst(ANDR | SETDR(3) | SETSR1(0) | SETSR2(3))
    self.assertEqual(0, self.sim.getReg(3))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(True, self.sim.getPcsrBit('z'))
    self.assertEqual(False, self.sim.getPcsrBit('p'))

  def test_AND1(self):  
    self.sim.doInst(ANDR | SETDR(4) | SETSR1(0) | SETSR2(1))
    self.assertEqual(0x1010, self.sim.getReg(4))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(True, self.sim.getPcsrBit('p'))

  def test_AND1(self):
    self.sim.doInst(ANDR | SETDR(4) | SETSR1(0) | SETSR2(2))
    self.assertEqual(0x1000, self.sim.getReg(4))
    self.assertEqual(False, self.sim.getPcsrBit('n'))
    self.assertEqual(False, self.sim.getPcsrBit('z'))
    self.assertEqual(True, self.sim.getPcsrBit('p'))    

class TestWatchPoint(unittest.TestCase):
  def setUp(self):
    self.sim = pylc3.simulator()

  def test_WatchRead(self):
    self.called = False
    def watch (a,p,c):
      self.called = True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.addWatchPoint(0x3000, True, False, watch)
    self.sim.stepN(5)
    self.assertEqual(True, self.called)

  def test_WatchWrite(self):
    self.called = False
    def watch (a,p,c):
      self.called = True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = ST | SETDR(1) | neg(-2) & 0x1FF
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.addWatchPoint(0x3000, False, True, watch)
    self.sim.stepN(5)
    self.assertEqual(True, self.called)

  def test_WatchReadWrite(self):
    self.called = 0
    def watch (a,p,c):
      self.called += 1
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = ST | SETDR(1) | neg(-2) & 0x1FF
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.addWatchPoint(0x3000, True, True, watch)
    self.sim.stepN(5)
    self.assertEqual(2, self.called)

class TestBreakPoint(unittest.TestCase):
  def setUp(self):
    self.sim = pylc3.simulator()

  def test_Break(self):
    self.called = False
    def watch (a):
      self.called = True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.addBreakPoint(0x3002, watch)
    self.sim.stepN(5)
    self.assertEqual(True, self.called)

  def test_NotBreak(self):
    self.called = False
    def watch (a):
      self.called = True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.addBreakPoint(0x2FFF, watch)
    self.sim.stepN(5)
    self.assertEqual(False, self.called)

class TestInterruptTrigger(unittest.TestCase):
  def setUp(self):
    self.sim = pylc3.simulator()

  def test_IntTrigger(self):
    self.called = False
    def trigger ():
      self.called = True
      return True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.mem[0x0180] = 0x1000
    self.sim.setPriority(3)
    self.sim.addInterruptTrigger(0x80, 7, trigger)
    self.sim.stepN(1)
    self.assertEqual(True, self.called)
    self.assertEqual(0x1000, self.sim.getPC())

  def test_NoIntTrigger(self):
    self.called = False
    def trigger ():
      self.called = True
      return True
    self.sim.setPC(0x3000)
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.mem[0x0181] = 0x1000
    self.sim.setPriority(7)
    self.sim.addInterruptTrigger(0x81, 7, trigger)
    self.sim.stepN(1)
    self.assertEqual(True, self.called)
    self.assertNotEqual(0x1000, self.sim.getPC())

class TestStep(unittest.TestCase):
  def setUp(self):
    self.sim = pylc3.simulator()

  def test_NoFunc(self):
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.setPC(0x3000)
    self.sim.stepN(1)
    self.assertEqual(0x3001, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(2)
    self.assertEqual(0x3002, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(3)
    self.assertEqual(0x3003, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(4)
    self.assertEqual(0x3004, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(5)
    self.assertEqual(0x3005, self.sim.getPC())

  def test_Func(self):
    self.sim.setReg(4,0x4000)
    self.sim.mem[0x3000] = JSRR | SETSR1(4)
    self.sim.mem[0x3001] = NOP
    self.sim.mem[0x3002] = NOP
    self.sim.mem[0x3003] = NOP
    self.sim.mem[0x3004] = NOP
    self.sim.mem[0x4000] = RET
    self.sim.setPC(0x3000)
    self.sim.stepN(1)
    self.assertEqual(0x4000, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(2)
    self.assertEqual(0x3001, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(3)
    self.assertEqual(0x3002, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(4)
    self.assertEqual(0x3003, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(5)
    self.assertEqual(0x3004, self.sim.getPC())


class TestNext(unittest.TestCase):
  def setUp(self):
    self.sim = pylc3.simulator()

  def test_NoFunc(self):
    self.sim.mem[0x3000] = 0
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.setPC(0x3000)
    self.sim.stepN(1)
    self.assertEqual(0x3001, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(2)
    self.assertEqual(0x3002, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(3)
    self.assertEqual(0x3003, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(4)
    self.assertEqual(0x3004, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.stepN(5)
    self.assertEqual(0x3005, self.sim.getPC())

  def test_Func(self):
    self.sim.setReg(4,0x4000)
    self.sim.mem[0x3000] = JSRR | SETSR1(4)
    self.sim.mem[0x3001] = 0
    self.sim.mem[0x3002] = 0
    self.sim.mem[0x3003] = 0
    self.sim.mem[0x3004] = 0
    self.sim.mem[0x4000] = RET
    self.sim.setPC(0x3000)
    self.sim.nextN(1)
    self.assertEqual(0x3001, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.nextN(2)
    self.assertEqual(0x3002, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.nextN(3)
    self.assertEqual(0x3003, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.nextN(4)
    self.assertEqual(0x3004, self.sim.getPC())
    self.sim.setPC(0x3000)
    self.sim.nextN(5)
    self.assertEqual(0x3005, self.sim.getPC())


doTest(TestAdd)
doTest(TestAnd)
doTest(TestWatchPoint)
doTest(TestBreakPoint)
doTest(TestInterruptTrigger)
doTest(TestStep)
doTest(TestNext)
