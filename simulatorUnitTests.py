#!/usr/bin/env python2.7

import pylc3
import unittest
from LC3Helper import *
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
    print(self.sim.getReg(3))
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
    self.assertEqual(false, self.sim.getPcsrBit('n'))
    self.assertEqual(true, self.sim.getPcsrBit('z'))
    self.assertEqual(false, self.sim.getPcsrBit('p'))

  def test_AND1(self):  
    self.sim.doInst(ANDR | SETDR(4) | SETSR1(0) | SETSR2(1))
    self.assertEqual(0x1010, self.sim.getReg(4))
    self.assertEqual(false, self.sim.getPcsrBit('n'))
    self.assertEqual(false, self.sim.getPcsrBit('z'))
    self.assertEqual(true, self.sim.getPcsrBit('p'))

  def test_AND1(self):
    self.sim.doInst(ANDR | SETDR(4) | SETSR1(0) | SETSR2(2))
    self.assertEqual(0x1000, self.sim.getReg(4))
    self.assertEqual(false, self.sim.getPcsrBit('n'))
    self.assertEqual(false, self.sim.getPcsrBit('z'))
    self.assertEqual(true, self.sim.getPcsrBit('p'))    

# if __name__ == '__main__':
  #If Prints OK then ALL TESTS PASSED
  # YAY 
  # unittest.main()

def doTest(testCase):
  print "-------------------------------------"

  suite = unittest.TestLoader().loadTestsFromTestCase(testCase)
  res = unittest.TextTestRunner(verbosity=2).run(suite)
  
  print "List of Test Errors", res.errors
  print "List of Test Failures", res.failures
  print "List of Test Unexpected Successes", res.unexpectedSuccesses
  print "Num Tests Run:", res.testsRun
  print "Was Successful ?:", res.wasSuccessful() 
  print "-------------------------------------"

doTest(TestAdd)
doTest(TestAnd)