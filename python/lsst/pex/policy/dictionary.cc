/*
 * LSST Data Management System
 * Copyright 2008-2016  AURA/LSST.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lsst/pex/policy/Dictionary.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/policy/PolicyFile.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace pex {
namespace policy {

PYBIND11_MODULE(dictionary, mod) {
    py::class_<Dictionary, std::shared_ptr<Dictionary>, Policy> clsDictionary(mod, "Dictionary");

    clsDictionary.def(py::init<>());
    clsDictionary.def(py::init<std::string &>());
    clsDictionary.def(py::init<const Dictionary &>());
    clsDictionary.def(py::init<PolicyFile &>());
    clsDictionary.def(py::init<const Policy &>());

    clsDictionary.def("getDefinitions", (Policy::Ptr (Dictionary::*)()) & Dictionary::getDefinitions);
    clsDictionary.def("check", &Dictionary::check);
    clsDictionary.def("definedNames",
                      (int (Dictionary::*)(std::list<std::string> &, bool) const) & Dictionary::definedNames,
                      "names"_a, "append"_a = false);
    clsDictionary.def("definedNames",
                      (Dictionary::StringArray (Dictionary::*)() const) & Dictionary::definedNames);
    clsDictionary.def("getDef", &Dictionary::getDef);
    clsDictionary.def("makeDef", &Dictionary::makeDef);
    clsDictionary.def("hasSubDictionary", &Dictionary::hasSubDictionary);
    clsDictionary.def("getSubDictionary", &Dictionary::getSubDictionary);

    // For some strange reason pybind11 doesn't like it if we use the default argument here
    clsDictionary.def("validate",
                      [](Dictionary const &self, Policy const &pol) { return self.validate(pol); }, "pol"_a);
    clsDictionary.def("validate", [](Dictionary const &self, Policy const &pol,
                                     ValidationError *errs) { self.validate(pol, errs); },
                      "pol"_a, "errs"_a);

    clsDictionary.def("loadPolicyFiles", (int (Dictionary::*)(bool)) & Dictionary::loadPolicyFiles,
                      "strict"_a = true);
    clsDictionary.def("loadPolicyFiles", [](Dictionary &self, std::string const &repository) {
        return self.loadPolicyFiles(repository);
    });
    clsDictionary.def("loadPolicyFiles", [](Dictionary &self, std::string const &repository, bool strict) {
        return self.loadPolicyFiles(repository, strict);
    });

    clsDictionary.def("getPrefix", &Dictionary::getPrefix);

    py::class_<Definition> clsDefinition(mod, "Definition");

    clsDefinition.def(py::init<const std::string &>(), "paramName"_a = "");

    clsDefinition.def("getName", &Definition::getName);
    clsDefinition.def("getPrefix", &Definition::getPrefix);
    clsDefinition.def("setPrefix", &Definition::setPrefix);
    clsDefinition.def("isChildDefinition", &Definition::isChildDefinition);
    clsDefinition.def("setChildDefinition", &Definition::setChildDefinition);
    clsDefinition.def("isWildcard", &Definition::isWildcard);
    clsDefinition.def("setWildcard", &Definition::setWildcard);
    clsDefinition.def("setName", &Definition::setName);
    clsDefinition.def("getData", &Definition::getData);
    clsDefinition.def("setData", &Definition::setData);
    clsDefinition.def("getType", &Definition::getType);
    clsDefinition.def("getTypeName", &Definition::getTypeName);
    clsDefinition.def("getDefault", &Definition::getDefault);
    clsDefinition.def("getDescription", &Definition::getDescription);
    clsDefinition.def("getMaxOccurs", &Definition::getMaxOccurs);
    clsDefinition.def("getMinOccurs", &Definition::getMinOccurs);
    clsDefinition.def("check", &Definition::check);
}

}  // policy
}  // pex
}  // lsst
