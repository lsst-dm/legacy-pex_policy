#!/usr/bin/env python
"""
Comprehensive tests reading and retrieving data of all types
"""
import os
import sys
import unittest
import time

import lsst.utils.tests as utilsTests
from lsst.pex.policy import Policy, PolicyStringDestination, PAFWriter

class PolicyOutStringTestCase(unittest.TestCase):

    def setUp(self):
        self.policy = Policy()
        self.policy.set("answer", 42)
        self.policy.set("name", "ray")

    def tearDown(self):
        del self.policy

    def testDest(self):
        dest = PolicyStringDestination("#<?cfg paf policy ?>")
        self.assertEquals(dest.getData(), "#<?cfg paf policy ?>")

    def testWrite(self):
        writer = PAFWriter()
        writer.write(self.policy, True)
        out = writer.toString();
        self.assert_(out.startswith("#<?cfg paf policy ?>"))

def suite():
    """Returns a suite containing all the test cases in this module."""

    utilsTests.init()

    suites = []
    suites += unittest.makeSuite(PolicyOutStringTestCase)
    suites += unittest.makeSuite(utilsTests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests"""
    utilsTests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
