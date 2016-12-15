#
# LSST Data Management System
# See COPYRIGHT file at the top of the source tree.
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
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the LSST License Statement and
# the GNU General Public License along with this program.  If not,
# see <http://www.lsstcorp.org/LegalNotices/>.
#

# -*- python -*-
"""
Regression tests for https://jira.lsstcorp.org/browse/DM-8654

See testDm8654.cc for tests of the C++ interface.

Run with:
   python testDm8654.py
or
   pytest testDm8654.py
"""

from __future__ import absolute_import, division, print_function

import unittest

import lsst.utils.tests
import lsst.pex.policy as pexPolicy


class Dm8654TestSuite(lsst.utils.tests.TestCase):

    def setUp(self):
        self._diversePolicy = pexPolicy.Policy()

        self._diversePolicy.set("answer", 42)

        self._topNames = {"answer"}

        self._diversePolicy.add("array", 2)
        self._diversePolicy.add("array", 3)
        self._diversePolicy.add("array", 5)
        self._diversePolicy.add("array", 8)

        self._topNames.add("array")

        self._diversePolicy.set("sub.logical", "false")
        self._diversePolicy.set("sub.answer", "42")
        self._diversePolicy.set("sub.stringlyTyped", "true")

        self._topNames.add("sub")
        self._fullNames = self._topNames | {"sub.logical", "sub.answer", "sub.stringlyTyped"}

    def tearDown(self):
        # For some reason Policies don't get garbage collected(at least in Swig)
        del self._diversePolicy

    def testNamesExpected(self):
        """Test if `Policy::names(bool)` returns heirarchical names if and
        only if they are requested.
        """
        self.assertEquals(set(self._diversePolicy.names()), self._fullNames)
        self.assertEquals(set(self._diversePolicy.names(True)), self._topNames)
        self.assertEquals(set(self._diversePolicy.names(False)), self._fullNames)


class MemoryTester(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()
