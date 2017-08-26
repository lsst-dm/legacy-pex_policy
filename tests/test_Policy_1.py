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

import unittest
import os.path

import lsst.utils.tests
from lsst.pex.policy import Policy, NameNotFound
import lsst.pex.exceptions
from past.builtins import long


proddir = lsst.utils.getPackageDir('pex_policy')

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


class PolicyTestCase(unittest.TestCase):
    def testPolicySetget(self):
        p = Policy()
        self.assertFalse(p.exists("foo"), "empty existence test failed")
        self.assertEqual(p.valueCount("foo.bar"), 0,
                         "empty valueCount test failed")

        self.assertRaises(lsst.pex.exceptions.Exception, p.getTypeInfo, "foo")

        p.set("doall", "true")

        # non-existence tests on a non-empty policy
        self.assertFalse(p.exists("foo"),
                         "non-empty non-existence test failed")
        self.assertEqual(p.valueCount("foo.bar"), 0,
                         "empty valueCount test failed")
        self.assertFalse(p.isInt("foo"),
                         "non-empty non-existence type test failed")
        self.assertRaises(lsst.pex.exceptions.Exception, p.getTypeInfo, "foo")

        # existence tests
        self.assertTrue(p.exists("doall"), "non-empty existence test failed")
        self.assertEqual(p.valueCount("doall"), 1,
                         "single valueCount test failed")

        self.assertRaises(lsst.pex.exceptions.Exception, p.getInt, "doall")
        self.assertRaises(lsst.pex.exceptions.Exception, p.getDoubleArray, "doall")

        self.assertEqual(p.get("doall"), "true",
                         "top-level getString failed")
        p.set("doall", "duh")
        self.assertEqual(p.get("doall"), "duh",
                         "top-level getString failed")

        # test array access
        ary = p.getArray("doall")
        self.assertEqual(len(ary), 1,
                         "scalar property has more than one value")

        self.assertEqual(ary[0], "duh", "scalar access via array failed")

        p.add("doall", "never")
        self.assertEqual(p.valueCount("doall"), 2,
                         "2-elem. valueCount test failed")
        self.assertEqual(p.get("doall"), "never", "top-level add failed")
        ary = p.getArray("doall")
        self.assertEqual(len(ary), 2,
                         "scalar property has wrong number of values")
        self.assertEqual(ary[0], "duh",
                         "scalar access via (2-el) array failed")
        self.assertEqual(ary[-1], "never",
                         "scalar access via (2-el) array failed")

        # test hierarchical access

        # list names

        # test types
        p.set("pint", 5)
        self.assertEquals(p.getInt("pint"), 5, "support for type int failed")
        self.assertIsInstance(p.get("pint"), (int, long),
                          "auto-typing for int failed")
        p.set("pdbl", 5.1)
        self.assertAlmostEqual(p.getDouble("pdbl"), 5.1, 7,
                               "support for type double failed")
        self.assertEqual(type(p.get("pdbl")), type(5.1),
                         "auto-typing for double failed")
        p.set("pbool", True)
        self.assertTrue(p.getBool("pbool"), "support for type bool failed")
        self.assertEqual(type(p.get("pbool")), type(True),
                         "auto-typing for bool failed")
        p.add("pbool", False)

        # test shallow & deep copies

        # test raise NameNotFound if not present
        self.assertRaises(NameNotFound, p.get, "nonexistent")
        self.assertRaises(NameNotFound, p.getArray, "nonexistent")
        self.assertRaises(NameNotFound, p.getDouble, "nonexistent")

    def testSimpleLoad(self):
        # n = mwid.Citizen_census(0)
        p = Policy.createPolicy(os.path.join(proddir, "examples", "EventTransmitter_policy.paf"))
        self.assertEqual(p.get("transmitter.serializationFormat"), "deluxe")
        p = None
        # self.assertEqual(mwid.Citizen_census(0), n, "Policy apparently leaked")

    def testEmptyPolicy(self):
        p = Policy()  # noqa F841: unused variable

    def testPolicyCopy(self):
        p = Policy.createPolicy(os.path.join(proddir, "examples", "EventTransmitter_policy.paf"))
        pp = Policy(p, True)
        self.assertEqual(p.get("transmitter.serializationFormat"), "deluxe")
        self.assertEqual(pp.getString("transmitter.serializationFormat"), "deluxe")
        p = None
        self.assertEqual(pp.getString("transmitter.serializationFormat"), "deluxe")

    def testSetNothing(self):
        p = Policy()
        try:
            p.set("foo", None)
            self.assertTrue(False, "Setting value to None succeeded.")
        except RuntimeError:
            self.assertFalse(p.exists("foo"))


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()

if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()
