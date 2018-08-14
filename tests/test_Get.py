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

"""
Comprehensive tests reading and retrieving data of all types
"""

import os
import unittest

import lsst.utils
import lsst.utils.tests
from lsst.pex.policy import Policy

proddir = lsst.utils.getPackageDir('pex_policy')


class GetTestCase(unittest.TestCase):

    def setUp(self):
        self.policyfile = os.path.join(proddir, "examples", "types.paf")
        self.policy = Policy.createPolicy(self.policyfile, False)

    def tearDown(self):
        del self.policy

    def testGet(self):
        p = self.policy
        self.assertEqual(p.get("int"), 0)
        self.assertIs(p.get("true"), True)
        self.assertIs(p.get("false"), False)
        self.assertAlmostEqual(p.get("dbl"), -0.05, 8)
        self.assertEqual(p.get("str"), "birthday")
        self.assertTrue(p.isFile("file"),
                        "Unexpected: 'file' is not a PolicyFile")
        self.assertIsNotNone(p.get("file"), "file value returned as None")
        self.assertEqual(p.get("file").getPath(), "CacheManager_dict.paf")
        self.assertTrue(p.isPolicy("pol"), "Unexpected: 'pol' is not a Policy")
        sp = p.get("pol")
        self.assertEqual(sp.get("int"), 2)

    def testGetIntArray(self):
        self.assertTrue(self.policy.isInt("int"))
        v = self.policy.getArray("int")
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = [-11, 0, 3, 42, -11, 0, 3, 42, 0, 0]
        self.assertEqual(len(v), len(truth), "wrong number of values in array")
        for i in range(len(truth)):
            self.assertEqual(v[i], truth[i],
                             "wrong array element at index %d: %d != %d" %
                             (i, v[i], truth[i]))

    def testGetBoolArray(self):
        self.assertTrue(self.policy.isBool("true"))
        v = self.policy.getArray("true")
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = [True]
        self.assertEqual(len(v), len(truth), "wrong number of values in array")
        for i in range(len(truth)):
            self.assertEqual(v[i], truth[i],
                             "wrong array element at index %i: %s != %s" %
                             (i, v[i], truth[i]))

        self.assertTrue(self.policy.isBool("false"))
        v = self.policy.getArray("false")
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = [False]
        self.assertEqual(len(v), len(truth), "wrong number of values in array")
        for i in range(len(truth)):
            self.assertEqual(v[i], truth[i],
                             "wrong array element at index %i: %s != %s" %
                             (i, v[i], truth[i]))

    def testGetDoublArray(self):
        self.assertTrue(self.policy.isDouble("dbl"))
        v = self.policy.getArray("dbl")
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = [-1.0, -65.78, -14.0, -0.12, -0.12, 1.0, 65.78, 14.0, 0.12,
                 0.12, 1.0, 65.78, 14.0, 0.12, 0.12, -1.0e10, -65780000.0,
                 -0.014, -0.12e14, 0.12e-11, 14.0, 0.12, 0.12, 1.0, 65.78,
                 14.0, 50000, -0.05]
        self.assertEqual(len(v), len(truth),
                         "wrong number of values in array: %i != %i" %
                         (len(v), len(truth)))
        for i in range(len(truth)):
            self.assertAlmostEqual(v[i], truth[i], 8,
                                   "wrong array element at index %d: %g != %g" %
                                   (i, v[i], truth[i]))

    def testGetStringArray(self):
        self.assertTrue(self.policy.isString("str"))
        v = self.policy.getArray("str")
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = ["word", "two words", "quoted ' words", 'quoted " words',
                 "a very long, multi-line description", "happy", "birthday"]
        self.assertEqual(len(v), len(truth), "wrong number of values in array")
        for i in range(len(truth)):
            self.assertEqual(v[i], truth[i],
                             "wrong array element at index %d: %s != %s" %
                             (i, v[i], truth[i]))

    def testGetEmptyString(self):
        p = self.policy
        self.assertEqual(p.get("empty"), '')
        s = p.getArray("empty")
        self.assertEqual(len(s), 5)
        self.assertEqual(s[0], ' description ')
        self.assertEqual(s[1], '  ')
        self.assertEqual(s[2], '  ')
        self.assertEqual(s[3], ' ')
        self.assertEqual(s[4], '')

    def testGetFileArray(self):
        self.assertTrue(self.policy.isFile("file"))
        v = self.policy.getArray("file")
        self.assertIsNotNone(v, "file array returned as None")

        # this is be fixed in another ticket
        self.assertIsInstance(v, list, "array value not returned as a list")

        truth = ["EventTransmitter_policy.paf", "CacheManager_dict.paf"]
        self.assertEqual(len(v), len(truth), "wrong number of values in array")
        for i in range(len(truth)):
            self.assertEqual(v[i].getPath(), truth[i],
                             "wrong array element at index %d: %s != %s" %
                             (i, v[i], truth[i]))

    def testGetPolicyArray(self):
        self.assertTrue(self.policy.isPolicy("pol"))
        v = self.policy.getArray("pol")
        # this is be fixed in another ticket
        self.assertIsInstance(v, list, "array value not returned as a list")

        self.assertEqual(len(v), 2, "wrong number of values in array")
        self.assertEqual(v[0].get("int"), 1)
        self.assertEqual(v[1].get("int"), 2)
        self.assertAlmostEqual(v[0].get("dbl"), 0.0003, 8)
        self.assertAlmostEqual(v[1].get("dbl"), -5.2, 8)


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


__all__ = "GetTestCase".split()


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()
