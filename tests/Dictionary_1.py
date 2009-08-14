import pdb                          # we may want to say pdb.set_trace()
import unittest
import inspect

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

    def assertRaiseLCE(self, excClass, excMsg, callableObj, failMsg, *args, **kwargs):
        """
        Expect callableObj(args, kwargs) to raise an LsstCppException that wraps
        the class specified by excClass, and carrying a message that contains
        excMsg.

        excClass: the subclass of LsstCppException we expect to see
        excMsg: a substring of the message it should carry
        callableObj: the thing that, when called, should raise an exception
        failMsg: the assertion message if this fails
        args, kwargs (optional): arguments to pass to callableObj
        """
        try:
            callableObj(*args, **kwargs)
        except LsstCppException, e:
            lce = "lsst.pex.exceptions.exceptionsLib.LsstCppException"
            self.assert_(str(e.__class__).find(lce) > 0,
                         failMsg + ": expected an " + lce + ", found a " 
                         + str(e.__class__))
            self.assert_(len(e.args) == 1)
            nested = e.args[0] # the real exception that was thrown
            self.assert_(str(nested.__class__).find(excClass) > 0,
                         failMsg + ": expected a " + excClass + "; found a " 
                         + str(nested.__class__))
            self.assert_(str(e).find(excMsg) > 0, 
                         failMsg + ": expected to see the message \"" + excMsg 
                         + "\"; actual message was \"" + str(e) + "\".")
#            print " -%%% e = ", e
#            print "    % members of e: ", dir(e)
#            print "    % members of len(e.args): ", len(e.args)
#            print "    % members of e.args.args[0]: ", inspect.getmembers(e.args[0])
#            print "    % inspect(e): ", inspect.getmembers(e)
        else:
            self.fail(failMsg + ": did not raise " + excClass)

    def testBadDictionary(self):
        # TODO: have this fail on load, instead of on inspection
        d = Dictionary("tests/dictionary/dictionary_bad_policyfile.paf")
        self.assertRaiseLCE("DictionaryError", "Illegal type: \"PolicyFile\"",
                            d.makeDef("file_type").getType,
                            "Dictionary specified PolicyFile type")

        d = Dictionary("tests/dictionary/dictionary_bad_unknown_type.paf")
        self.assertRaiseLCE("DictionaryError", "Unknown type: \"NotAType\"",
                            d.makeDef("something").getType,
                            "Dictionary specifies unknown types")

        d = Dictionary("tests/dictionary/dictionary_bad_type_type.paf")
        self.assertRaiseLCE("DictionaryError", "Expected string",
                            d.makeDef("something").getType,
                            "Expected string \"type\" type")

        d = Dictionary("tests/dictionary/dictionary_bad_multiple_definitions.paf")
        self.assertRaiseLCE("DictionaryError", "expected a single", d.check, 
                            "Dictionary has two 'definitions' sections")

        p = Policy("tests/dictionary/values_policy_good_1.paf")
        d = Dictionary("tests/dictionary/dictionary_bad_multiple_min.paf")
        self.assertRaiseLCE("DictionaryError", "Min value for int_ra", d.validate,
                            "Two mins specified.", p)
        d = Dictionary("tests/dictionary/dictionary_bad_multiple_max.paf")
        self.assertRaiseLCE("DictionaryError", "Max value for int_ra", d.validate,
                            "Two maxes specified.", p)
        d = Dictionary("tests/dictionary/dictionary_bad_min_wrong_type.paf")
        self.assertRaiseLCE("DictionaryError",
                            "Wrong type for int_range_count_type min",
                            d.validate, "Wrong min type.", p)
        d = Dictionary("tests/dictionary/dictionary_bad_max_wrong_type.paf")
        self.assertRaiseLCE("DictionaryError",
                            "Wrong type for int_range_count_type max",
                            d.validate, "Wrong max type.", p)

    def testSimpleValidate(self):
        d = Dictionary("tests/dictionary/simple_dictionary.paf")
        p = Policy("tests/dictionary/simple_policy.paf")
        ve = ValidationError("Dictionary_1.py", 0, "testSimpleValidate")
        d.validate(p, ve)
        self.assert_(ve.getErrors("name") == 0, "no errors in name")
        self.assert_(ve.getErrors("height") == 0, "no errors in height")
        self.assert_(ve.getErrors() == 0, "no errors overall")

    def testTypeValidation(self):
        d = Dictionary("tests/dictionary/types_dictionary.paf")
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

        p = Policy("tests/dictionary/types_policy_good.paf")

        ve = ValidationError("Dictionary_1.py", 0, "testTypeValidation")
        d.validate(p, ve)
        self.assert_(ve.getErrors("file_type") == ValidationError.NOT_LOADED,
                     "require loadPolicyFiles before validating")
        self.assert_(ve.getErrors("undef_file") == ValidationError.NOT_LOADED,
                     "require loadPolicyFiles before validating")
        self.assert_(ve.getErrors() == ValidationError.NOT_LOADED, "no other errors")
        self.assert_(ve.getParamCount() == 2, "no other errors")

        p.loadPolicyFiles()
        ve = ValidationError("Dictionary_1.py", 0, "testTypeValidation")
        d.validate(p, ve)

        self.assert_(ve.getErrors("undef_type")  == 0, "no errors with undef")
        self.assert_(ve.getErrors("int_type")    == 0, "no errors with int")
        self.assert_(ve.getErrors("double_type") == 0, "no errors with double")
        self.assert_(ve.getErrors("bool_type")   == 0, "no errors with bool")
        self.assert_(ve.getErrors("string_type") == 0, "no errors with string")
        self.assert_(ve.getErrors("policy_type") == 0, "no errors with policy")
        self.assert_(ve.getErrors("file_type")   == 0, "no errors with file")
        self.assert_(ve.getErrors() == 0, "no errors overall")

    def testWrongType(self):
        d = Dictionary("tests/dictionary/types_dictionary.paf")

        p = Policy("tests/dictionary/types_policy_bad_bool.paf")
        ve = ValidationError("Dictionary_1.py", 0, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getParamCount() == 5, "number of errors")
        self.assert_(ve.getErrors("bool_type") == 0, "correct type")
        
        p = Policy("tests/dictionary/types_policy_bad_int.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getParamCount() == 5, "number of errors")
        self.assert_(ve.getErrors("int_type") == 0, "correct type")
        self.assert_(ve.getErrors("double_type") == ValidationError.WRONG_TYPE,
                     "int can't pass as double")
        
        p = Policy("tests/dictionary/types_policy_bad_double.paf")
        ve = ValidationError("Dictionary_1.py", 2, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getParamCount() == 5, "number of errors")
        self.assert_(ve.getErrors("double_type") == 0, "correct type")

        p = Policy("tests/dictionary/types_policy_bad_string.paf")
        ve = ValidationError("Dictionary_1.py", 3, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getParamCount() == 5, "number of errors")
        self.assert_(ve.getErrors("string_type") == 0, "correct type")

        p = Policy("tests/dictionary/types_policy_bad_policy.paf")
        ve = ValidationError("Dictionary_1.py", 4, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getParamCount() == 4, "number of errors")
        self.assert_(ve.getErrors("policy_type") == 0, "correct type")
        self.assert_(ve.getErrors("file_type") == 0, "correct type")

        p = Policy("tests/dictionary/types_policy_bad_file.paf")
        ve = ValidationError("Dictionary_1.py", 5, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.NOT_LOADED, "not loaded")
        self.assert_(ve.getParamCount() == 6, "number of errors")
        self.assert_(ve.getErrors("bool_type") == ValidationError.NOT_LOADED,
                     "not loaded")
        self.assert_(ve.getErrors("file_type") == ValidationError.NOT_LOADED,
                     "not loaded")
        self.assert_(ve.getErrors("policy_type") == ValidationError.NOT_LOADED,
                     "not loaded")
        p.loadPolicyFiles()
        ve = ValidationError("Dictionary_1.py", 6, "testWrongType")
        d.validate(p, ve);
        self.assert_(ve.getErrors() == ValidationError.WRONG_TYPE, "wrong type")
        self.assert_(ve.getErrors("file_type") == 0, "correct type")
        self.assert_(ve.getErrors("policy_type") == 0, "correct type")
        self.assert_(ve.getParamCount() == 4, "wrong type")

        # TODO: test missing elements (minOccurs >= 1)
        # TODO: test bad dictionary (wrong types for allowed values, etc.)

    def testValues(self):
        d = Dictionary("tests/dictionary/values_dictionary.paf")
        d.check()
        p = Policy("tests/dictionary/values_policy_good_1.paf")
        ve = ValidationError("Dictionary_1.py", 0, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 0)

        # policy: disallow allowed, min, max
        p = Policy("tests/dictionary/values_policy_bad_policy_set.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getErrors("policy_set_type") 
                     == ValidationError.VALUE_DISALLOWED)
        p = Policy("tests/dictionary/values_policy_bad_policy_max.paf")
        ve = ValidationError("Dictionary_1.py", 2, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getErrors("policy_max_type") 
                     == ValidationError.VALUE_OUT_OF_RANGE)
        p = Policy("tests/dictionary/values_policy_bad_policy_min.paf")
        ve = ValidationError("Dictionary_1.py", 3, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getErrors("policy_min_type") 
                     == ValidationError.VALUE_OUT_OF_RANGE)

        # minOccurs/maxOccurs
        p = Policy("tests/dictionary/values_policy_bad_occurs.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 5)
        self.assert_(ve.getErrors("bool_set_count_type")
                    == ValidationError.TOO_MANY_VALUES)
        self.assert_(ve.getErrors("int_range_count_type")
                    == ValidationError.MISSING_REQUIRED)
        self.assert_(ve.getErrors("double_range_count_type")
                    == ValidationError.TOO_MANY_VALUES)
        self.assert_(ve.getErrors("string_count_type")
                    == ValidationError.ARRAY_TOO_SHORT)
        self.assert_(ve.getErrors("policy_count_type")
                    == ValidationError.TOO_MANY_VALUES)

        # min
        p = Policy("tests/dictionary/values_policy_bad_min.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 4)
        self.assert_(ve.getErrors() == ValidationError.VALUE_OUT_OF_RANGE)
        self.assert_(ve.getErrors("string_count_type") == ValidationError.OK)
        self.assert_(ve.getErrors("policy_count_type") == ValidationError.OK)
        # max
        p = Policy("tests/dictionary/values_policy_bad_max.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 4)
        self.assert_(ve.getErrors() == ValidationError.VALUE_OUT_OF_RANGE)
        self.assert_(ve.getErrors("string_count_type") == ValidationError.OK)
        self.assert_(ve.getErrors("policy_count_type") == ValidationError.OK)

        # allowed
        p = Policy("tests/dictionary/values_policy_bad_allowed.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 4)
        self.assert_(ve.getErrors() == ValidationError.VALUE_DISALLOWED)
        self.assert_(ve.getErrors("int_range_count_type") == ValidationError.OK)
        self.assert_(ve.getErrors("string_count_type") == ValidationError.OK)
        self.assert_(ve.getErrors("policy_count_type") == ValidationError.OK)

        # allowed & min/max
        p = Policy("tests/dictionary/values_policy_bad_allowedminmax.paf")
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)
        self.assert_(ve.getParamCount() == 2)
        self.assert_(ve.getErrors("int_range_set_type") 
                     == ValidationError.VALUE_DISALLOWED 
                     + ValidationError.VALUE_OUT_OF_RANGE)
        self.assert_(ve.getErrors("double_range_count_type") 
                     == ValidationError.TOO_MANY_VALUES 
                     + ValidationError.VALUE_OUT_OF_RANGE)
        ve = ValidationError("Dictionary_1.py", 1, "testValues")
        d.validate(p, ve)

def suite():
    """a suite containing all the test cases in this module"""
    tests.init()

    suites = []
    suites += unittest.makeSuite(DictionaryTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)

    return unittest.TestSuite(suites)

if __name__ == "__main__":
    tests.run(suite())
