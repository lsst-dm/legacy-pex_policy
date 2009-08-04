import pdb                          # we may want to say pdb.set_trace()
import unittest

import lsst.utils.tests as tests
from lsst.pex.policy import Policy
from lsst.pex.policy import Policy, Dictionary, PolicyFile
from lsst.pex.exceptions import LsstCppException

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

class DictionaryTestCase(unittest.TestCase):
    def testDictionaryLoad(self):
        d = Dictionary()
        df = PolicyFile("examples/simple_dictionary.paf")
        self.assert_(not d.isDictionary(), "false positive dictionary")
        df.load(d)
        self.assert_(d.isDictionary(), "failed to recognize a dictionary")

def suite():
    """a suite containing all the test cases in this module"""
    tests.init()

    suites = []
    suites += unittest.makeSuite(DictionaryTestCase)

    return unittest.TestSuite(suites)

if __name__ == "__main__":
    tests.run(suite())
