import pdb
import unittest
import math

import lsst.utils.tests as tests
from lsst.pex.policy import Policy, PolicyFile, Dictionary, ValidationError
from lsst.pex.exceptions import LsstCppException

sd = Dictionary()
sp = Policy()
spe = Policy()
PolicyFile("tests/dictionary/simple_dictionary.paf").load(sd)
PolicyFile("tests/dictionary/simple_policy.paf").load(sp)
PolicyFile("tests/dictionary/simple_policy_error.paf").load(spe)
ve = ValidationError("noFile", 0, "noFunction")

# sd.validate(sp, ve)
