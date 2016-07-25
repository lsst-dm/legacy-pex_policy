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

using namespace lsst::pex::policy;

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_dictionary) {
    py::module mod("_dictionary", "Access to the classes from the pex policy Dictionary library");

    py::class_<Dictionary, std::shared_ptr<Dictionary>> clsDictionary(mod, "Dictionary", py::base<Policy>());

    clsDictionary.def(py::init<>());
    clsDictionary.def(py::init<std::string&>());
    clsDictionary.def(py::init<const Dictionary&>());
    clsDictionary.def(py::init<PolicyFile&>());
    clsDictionary.def(py::init<const Policy&>());

    clsDictionary.def("getDefinitions", (Policy::Ptr (Dictionary::*)()) &Dictionary::getDefinitions);
    clsDictionary.def("check", &Dictionary::check);
    clsDictionary.def("definedNames", (int (Dictionary::*)(std::list<std::string>&, bool) const) &Dictionary::definedNames,
        py::arg("names"), py::arg("append")=false);
    clsDictionary.def("definedNames", (Dictionary::StringArray (Dictionary::*)() const) &Dictionary::definedNames);
    clsDictionary.def("getDef", &Dictionary::getDef);
    clsDictionary.def("makeDef", &Dictionary::makeDef);
    clsDictionary.def("hasSubDictionary", &Dictionary::hasSubDictionary);
    clsDictionary.def("getSubDictionary", &Dictionary::getSubDictionary);

    // For some strange reason pybind11 doesn't like it if we use the default argument here
    clsDictionary.def("validate", [](Dictionary &d, const Policy& p){
        return d.validate(p);
    }, py::arg("pol"));
    clsDictionary.def("validate", [](Dictionary &d, const Policy& p, ValidationError *errs){
        d.validate(p, errs);
    }, py::arg("pol"), py::arg("errs"));

    clsDictionary.def("loadPolicyFiles", (int (Dictionary::*)(bool)) &Dictionary::loadPolicyFiles,
        py::arg("strict")=true);
    clsDictionary.def("loadPolicyFiles", [](Dictionary &d, const std::string& repository){
        return d.loadPolicyFiles(repository);
    });
    clsDictionary.def("loadPolicyFiles", [](Dictionary &d, const std::string& repository, bool strict){
        return d.loadPolicyFiles(repository, strict);
    });

    clsDictionary.def("getPrefix", &Dictionary::getPrefix);

    py::class_<Definition> clsDefinition(mod, "Definition");

    clsDefinition.def(py::init<const std::string&>(), py::arg("paramName")="");

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

    return mod.ptr();
}
