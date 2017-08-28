import unittest
import lsst.utils.tests


class UtilsBinaryTester(lsst.utils.tests.ExecutablesTestCase):
    pass


EXECUTABLES = ("DefaultPolicyFile_1", "PAFBadSyntax", "PAFtypes",
               "PolicyString_1", "Policy_1", "Policy_2", "Policy_3",
               "Policy_4", "testDefaults")
UtilsBinaryTester.create_executable_tests(__file__, EXECUTABLES)

if __name__ == "__main__":
    unittest.main()
