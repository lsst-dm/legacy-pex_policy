#!/usr/bin/env python
#
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
#
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the LSST License Statement and
# the GNU General Public License along with this program.  If not,
# see <http://www.lsstcorp.org/LegalNotices/>.
#

from builtins import str
import os
import unittest

import lsst.utils.tests

from lsst.pex.policy import Policy, UrnPolicyFile, BadNameError
import lsst.pex.exceptions

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

# TODO: test cross-package loading more thoroughly -- mix up the packages and
# repositories in a deeply nested and linked policy file.


class UrnPolicyFileTestCase(unittest.TestCase):
    examplesDir = None

    def assertRaiseLCE(self, excClass, excMsg, callableObj, failMsg, *args, **kwargs):
        """
        Expect callableObj(args, kwargs) to raise an exception of type excClass,
        and carres a message that contains excMsg.

        excClass: the subclass of LsstCppException we expect to see
        excMsg: a substring of the message it should carry
        callableObj: the thing that, when called, should raise an exception
        failMsg: the assertion message if this fails
        args, kwargs (optional): arguments to pass to callableObj
        """
        try:
            callableObj(*args, **kwargs)
        except excClass as e:
            self.assert_(str(e).find(excMsg) > 0,
                         failMsg + ": expected to see the message \"" + excMsg +
                         "\"; actual message was \"" + str(e) + "\".")
        else:
            self.fail(failMsg + ": did not raise " + excClass)

    def getExamples(self, filename=None):
        if not self.examplesDir:
            # XXX is this really the best way to find the src_dir?
            pexPolicyDir = lsst.utils.getPackageDir('pex_policy')
            self.examplesDir = os.path.join(pexPolicyDir, "examples")
        if filename:
            return os.path.join(self.examplesDir, filename)
        else:
            return self.examplesDir

    def testReference(self):
        addr = "pex_policy:examples:EventTransmitter_policy.paf"
        p = Policy()

        p.set("transmitter.logVerbosity", "not")
        UrnPolicyFile(addr).load(p)
        self.assert_(p.get("transmitter.logVerbosity") == "debug")

        p.set("transmitter.logVerbosity", "not")
        UrnPolicyFile("urn:eupspkg:" + addr).load(p)
        self.assert_(p.get("transmitter.logVerbosity") == "debug")

        p.set("transmitter.logVerbosity", "not")
        UrnPolicyFile("@@" + addr).load(p)
        self.assert_(p.get("transmitter.logVerbosity") == "debug")

    def testIndirect(self):
        urn = "@urn:eupspkg:pex_policy:tests/urn:indirect_parent_good.paf"
        p = Policy(urn)
        self.assert_(p.get("urn_full.name") == "Simple Policy")
        self.assert_(p.get("urn_brief.name") == "Simple Policy")
        self.assert_(p.get("urn_mixed_case.name") == "Simple Policy")
        self.assert_(p.get("local.foo") == "bar")

        p = Policy()
        UrnPolicyFile("pex_policy:tests/urn:level_1.paf").load(p)
        self.assert_(p.get("foo.bar.baz.qux.quux") == "schmazzle")

    def testLoading(self):
        p = Policy("urn:eupspkg:pex_policy:tests/urn:level_1.paf")
        self.assert_(p.get("foo.bar.baz.qux.quux") == "schmazzle")

        self.assertRaiseLCE(BadNameError, "Wrong number of terms",
                            Policy, "URN too short",
                            "urn:eupspkg:foo.paf")
        self.assertRaiseLCE(lsst.pex.exceptions.IoError, "failure opening Policy file",
                            Policy, "URN abbrev '@' not allowed in constructor",
                            "@pex_policy:tests/urn:level_1.paf")

        urn = "urn:eupspkg:pex_policy:tests/dictionary:defaults_dictionary_good.paf"
        self.assertRaiseLCE(lsst.pex.exceptions.IoError, "/./defaults_dictionary_indirect",
                            Policy.createPolicyFromUrn,
                            "doesn't support loading undecorated DictionaryFile",
                            urn)
        urn = "urn:eupspkg:pex_policy:tests/dictionary:defaults_dictionary_partial.paf"
        p = Policy.createPolicyFromUrn(urn)
        # make sure all reference types worked
        # self.assert_(p.get("indirect.string_type") == "foo")
        # self.assert_(p.get("indirect2.string_type") == "foo")
        self.assert_(p.get("indirect3.string_type") == "foo")
        self.assert_(p.get("indirect4.string_type") == "foo")

    def testTypos(self):
        base = "pex_policy:tests/urn:indirect_parent_typo_"
        self.assertRaiseLCE(lsst.pex.exceptions.IoError, "failure opening Policy file",
                            UrnPolicyFile(base + "1.paf").load, "Typo in URN",
                            Policy())
        self.assertRaiseLCE(lsst.pex.exceptions.IoError, "failure opening Policy file",
                            UrnPolicyFile(base + "2.paf").load, "Typo in URN",
                            Policy())

    def testRepos(self):
        # when the repository is mis-specified, local files cannot be loaded
        upf = UrnPolicyFile("pex_policy:tests:urn/indirect_parent_good.paf")
        # we expect it to look in <package>/tests/simple.paf
        pexPolicyDir = lsst.utils.getPackageDir('pex_policy')
        expectedFile = pexPolicyDir + "/tests/simple.paf"
        self.assertRaiseLCE(lsst.pex.exceptions.IoError,
                            "failure opening Policy file: " + expectedFile,
                            upf.load, "Wrong repository dir.", Policy())

        # a PAF file designed to have "tests" as it repository
        p = Policy()
        UrnPolicyFile("pex_policy:tests:urn/local_tests_repos.paf").load(p)
        self.assert_(p.get("local.polish") == "fancy")


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()

if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()
