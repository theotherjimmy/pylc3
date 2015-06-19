#!/usr/bin/env python2.7

import unittest

def doTest(testCase):

  suite = unittest.TestLoader().loadTestsFromTestCase(testCase)
  res = unittest.TextTestRunner(verbosity=2).run(suite)

def doTestVerbose(testCase):
  print "-------------------------------------"

  suite = unittest.TestLoader().loadTestsFromTestCase(testCase)
  res = unittest.TextTestRunner(verbosity=2).run(suite)

  print "List of Test Errors", res.errors
  print "List of Test Failures", res.failures
  print "List of Test Unexpected Successes", res.unexpectedSuccesses
  print "Num Tests Run:", res.testsRun
  print "Was Successful ?:", res.wasSuccessful()
  print "-------------------------------------"
