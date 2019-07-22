// -*- LSST-C++ -*-

/*
 * LSST Data Management System
 * See COPYRIGHT file at the top of the source tree.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program. If not,
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */

#include <list>
#include <set>
#include <string>

#define BOOST_TEST_MODULE Dm8654Cpp

#include "boost/test/included/unit_test.hpp"

#include "lsst/pex/policy/Policy.h"

/*
 * Regression tests for https://jira.lsstcorp.org/browse/DM-8654
 *
 * See testDm8654.py for tests of the Python interface.
 */
namespace lsst {
namespace pex {
namespace policy {

    /**
     * Common test data. Must include heirarchical keys.
     */
    struct NameFixture {
        NameFixture() : diversePolicy(), topNames(), fullNames() {
            diversePolicy.set("answer", 42);

            topNames.emplace("answer");

            diversePolicy.add("array", 2);
            diversePolicy.add("array", 3);
            diversePolicy.add("array", 5);
            diversePolicy.add("array", 8);

            topNames.emplace("array");

            diversePolicy.set("sub.logical", "false");
            diversePolicy.set("sub.answer", "42");
            diversePolicy.set("sub.stringlyTyped", "true");

            topNames.emplace("sub");
            fullNames.insert(topNames.begin(), topNames.end());
            fullNames.insert({"sub.logical", "sub.answer", "sub.stringlyTyped"});
        }

        template <class Iterable>
        std::set<typename Iterable::value_type> setOf(Iterable const & container) {
            return std::set<typename Iterable::value_type>(std::begin(container), std::end(container));
        }

        template <class Iterable>
        std::list<typename Iterable::value_type> listOf(Iterable const & container) {
            return std::list<typename Iterable::value_type>(std::begin(container), std::end(container));
        }

        Policy diversePolicy;
        std::set<std::string> topNames;
        std::set<std::string> fullNames;
    };

    /**
     * Test whether `Policy::names(bool)` and
     * `Policy::names(list<string>&, bool, bool)` behave identically.
     */
    BOOST_FIXTURE_TEST_CASE(namesMatch, NameFixture)
    {
        std::list<std::string> defaultNames, trueNames, falseNames;
        diversePolicy.names(defaultNames);
        diversePolicy.names(trueNames, true);
        diversePolicy.names(falseNames, false);

        BOOST_TEST(listOf(diversePolicy.names()) == defaultNames);
        BOOST_TEST(listOf(diversePolicy.names(true)) == trueNames);
        BOOST_TEST(listOf(diversePolicy.names(false)) == falseNames);
    }

    /**
     * Test whether `Policy::names(bool)` returns heirarchical names if and
     * only if they are requested.
     */
    BOOST_FIXTURE_TEST_CASE(namesExpected, NameFixture)
    {
        BOOST_TEST(setOf(diversePolicy.names()) == fullNames);
        BOOST_TEST(setOf(diversePolicy.names(true)) == topNames);
        BOOST_TEST(setOf(diversePolicy.names(false)) == fullNames);
    }

}}} /* namespace lsst::pex::policy */

