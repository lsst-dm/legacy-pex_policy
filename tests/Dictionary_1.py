import pdb                          # we may want to say pdb.set_trace()
import unittest

import lsst.utils.tests as tests
from lsst.pex.policy import Policy
from lsst.pex.policy import Policy, Dictionary, PolicyFile
from lsst.pex.policy import ValidationError, DictionaryError
from lsst.pex.exceptions import LsstCppException

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

class DictionaryTestCase(unittest.TestCase):
    def testDictionaryLoad(self):
        d = Dictionary()
        df = PolicyFile("tests/dictionary/simple_dictionary.paf")
        self.assert_(not d.isDictionary(), "false positive dictionary")
        df.load(d)
        self.assert_(d.isDictionary(), "failed to recognize a dictionary")

    def testBadDictionary(self):
        d = Dictionary()

        try:
            # TODO: have this fail on load, instead of on inspection
            PolicyFile("tests/dictionary/dictionary_policyfile.paf").load(d)
            d.makeDef("file_type").getType()
        except LsstCppException: #DictionaryError:
            pass
        else:
            self.fail("Dictionary cannot specify PolicyFile type.")

        try:
            # TODO: have this fail on load, instead of on inspection
            PolicyFile("tests/dictionary/dictionary_unknown_type.paf").load(d)
            d.makeDef("something").getType()
        except LsstCppException: #DictionaryError:
            pass
        else:
            self.fail("Dictionary cannot specify unknown types.")

    def testSimpleValidate(self):
        d = Dictionary()
        PolicyFile("tests/dictionary/simple_dictionary.paf").load(d)
        p = Policy()
        PolicyFile("tests/dictionary/simple_policy.paf").load(p)
        ve = ValidationError("Dictionary_1.py", 0, "testSimpleValidate")
        d.validate(p, ve)
        self.assert_(ve.getErrors("name") == 0, "no errors in name")
        self.assert_(ve.getErrors("height") == 0, "no errors in height")
        self.assert_(ve.getErrors() == 0, "no errors overall")

    def testTypeValidation(self):
        d = Dictionary()
        PolicyFile("tests/dictionary/types_dictionary.paf").load(d)
        self.assert_(d.makeDef("undef_type") .getType() == Policy.UNDEF,
                     "UNDEF definition type")
        self.assert_(d.makeDef("bool_type")  .getType() == Policy.BOOL,
                     "BOOL definition type")
        self.assert_(d.makeDef("int_type")   .getType() == Policy.INT,
                     "INT definition type")
        self.assert_(d.makeDef("double_type").getType() == Policy.DOUBLE,
                     "DOUBLE definition type")
        self.assert_(d.makeDef("string_type").getType() == Policy.STRING,
                     "STRING definition type")
        self.assert_(d.makeDef("policy_type").getType() == Policy.POLICY,
                     "POLICY definition type")
        self.assert_(d.makeDef("file_type").getType() == Policy.POLICY,
                     "POLICY definition type (substituted for PolicyFile)")

        p = Policy()
        PolicyFile("tests/dictionary/types_policy_good.paf").load(p)

        ve = ValidationError("Dictionary_1.py", 0, "testTypeValidation")
        d.validate(p, ve)
        self.assert_(ve.getErrors("file_type") == 512,
                     "require loadPolicyFiles before validating")
        self.assert_(ve.getParamCount() == 1, "no other errors")

        p.loadPolicyFiles()
        ve = ValidationError("Dictionary_1.py", 0, "testTypeValidation")
        d.validate(p, ve)

        self.assert_(ve.getErrors("undef_type") == 0, "no errors with undef")
        self.assert_(ve.getErrors("int_type") == 0, "no errors with int")
        self.assert_(ve.getErrors("double_type") == 0, "no errors with double")
        self.assert_(ve.getErrors("bool_type") == 0, "no errors with bool")
        self.assert_(ve.getErrors("string_type") == 0, "no errors with string")
        self.assert_(ve.getErrors("policy_type") == 0, "no errors with policy")
        self.assert_(ve.getErrors("file_type") == 0, "no errors with file")
        self.assert_(ve.getErrors() == 0, "no errors overall")

def suite():
    """a suite containing all the test cases in this module"""
    tests.init()

    suites = []
    suites += unittest.makeSuite(DictionaryTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)

    return unittest.TestSuite(suites)

if __name__ == "__main__":
    tests.run(suite())
