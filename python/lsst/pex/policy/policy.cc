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
#include "pybind11/operators.h"
#include "pybind11/stl.h"

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/policy/PolicyFile.h"

#include "lsst/pex/exceptions/Exception.h"
#include "lsst/pex/exceptions/python/Exception.h"
#include "lsst/pex/policy/Dictionary.h"

namespace py = pybind11;

using namespace lsst::pex::policy;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_policy) {
    using lsst::pex::policy::BadNameError;
    using lsst::pex::policy::DictionaryError;
    using lsst::pex::policy::NameNotFound;
    using lsst::pex::policy::TypeError;
    using lsst::pex::policy::ValidationError;

    py::module mod("_policy", "Access to the classes from the pex policy Policy library");

    lsst::pex::exceptions::python::declareException<BadNameError, lsst::pex::exceptions::RuntimeError>(mod, "BadNameError", "RuntimeError");
    lsst::pex::exceptions::python::declareException<DictionaryError, lsst::pex::exceptions::DomainError>(mod, "DictionaryError", "DomainError");
    lsst::pex::exceptions::python::declareException<NameNotFound, lsst::pex::exceptions::NotFoundError>(mod, "NameNotFound", "NotFoundError");
    lsst::pex::exceptions::python::declareException<TypeError, lsst::pex::exceptions::DomainError>(mod, "TypeError", "DomainError");
    auto clsValidationError = lsst::pex::exceptions::python::declareException<ValidationError>(mod, "ValidationError", "LogicError");

    py::enum_<ValidationError::ErrorType>(clsValidationError, "ErrorType")
        .value("OK", ValidationError::ErrorType::OK)
        .value("WRONG_TYPE", ValidationError::ErrorType::WRONG_TYPE)
        .value("MISSING_REQUIRED", ValidationError::ErrorType::MISSING_REQUIRED)
        .value("NOT_AN_ARRAY", ValidationError::ErrorType::NOT_AN_ARRAY)
        .value("ARRAY_TOO_SHORT", ValidationError::ErrorType::ARRAY_TOO_SHORT)
        .value("TOO_FEW_VALUES", ValidationError::ErrorType::TOO_FEW_VALUES)
        .value("TOO_MANY_VALUES", ValidationError::ErrorType::TOO_MANY_VALUES)
        .value("WRONG_OCCURRENCE_COUNT", ValidationError::ErrorType::WRONG_OCCURRENCE_COUNT)
        .value("VALUE_DISALLOWED", ValidationError::ErrorType::VALUE_DISALLOWED)
        .value("VALUE_OUT_OF_RANGE", ValidationError::ErrorType::VALUE_OUT_OF_RANGE)
        .value("BAD_VALUE", ValidationError::ErrorType::BAD_VALUE)
        .value("UNKNOWN_NAME", ValidationError::ErrorType::UNKNOWN_NAME)
        .value("BAD_DEFINITION", ValidationError::ErrorType::BAD_DEFINITION)
        .value("NOT_LOADED", ValidationError::ErrorType::NOT_LOADED)
        .value("UNKNOWN_ERROR", ValidationError::ErrorType::UNKNOWN_ERROR)
        .export_values();

    clsValidationError.def(py::init<const std::string&>());
    clsValidationError.def(py::init<char const*, int, char const*>());

    clsValidationError.def_readonly_static("EMPTY", &ValidationError::EMPTY);
    clsValidationError.def("getErrorMessageFor", &ValidationError::getErrorMessageFor);
    clsValidationError.def("getParamCount", &ValidationError::getParamCount);
    clsValidationError.def("paramNames", &ValidationError::paramNames);
    clsValidationError.def("getParamNames", &ValidationError::getParamNames);
    clsValidationError.def("getErrors", (int (ValidationError::*)(const std::string&) const) &ValidationError::getErrors);
    clsValidationError.def("getErrors", (int (ValidationError::*)() const) &ValidationError::getErrors);
    clsValidationError.def("describe", &ValidationError::describe);
    clsValidationError.def("what", &ValidationError::what);

    py::class_<Policy, std::shared_ptr<Policy>> clsPolicy(mod, "Policy");

    py::enum_<Policy::ValueType>(clsPolicy, "ValueType")
        .value("UNDETERMINED", Policy::ValueType::UNDETERMINED)
        .value("UNDEF", Policy::ValueType::UNDEF)
        .value("BOOL", Policy::ValueType::BOOL)
        .value("INT", Policy::ValueType::INT)
        .value("DOUBLE", Policy::ValueType::DOUBLE)
        .value("STRING", Policy::ValueType::STRING)
        .value("POLICY", Policy::ValueType::POLICY)
        .value("FILE", Policy::ValueType::FILE)
        .export_values();

    clsPolicy.def(py::init<>());
    clsPolicy.def(py::init<bool, const Dictionary&>());
    clsPolicy.def(py::init<const std::string&>());
    clsPolicy.def(py::init<Policy&, bool>(),
        py::arg("pol"), py::arg("deep")=false);

    clsPolicy.def_static("createPolicyFromUrn", &Policy::createPolicyFromUrn,
        py::arg("urn"), py::arg("validate")=true);
    clsPolicy.def_static("createPolicy", (Policy* (*)(PolicySource&, bool, bool)) &Policy::createPolicy,
        py::arg("input"), py::arg("doIncludes")=true, py::arg("validate")=true);
    clsPolicy.def_static("createPolicy", (Policy* (*)(const std::string&, bool, bool)) &Policy::createPolicy,
        py::arg("input"), py::arg("doIncludes")=true, py::arg("validate")=true);
    clsPolicy.def_static("createPolicy", (Policy* (*)(PolicySource&, const std::string&, bool)) &Policy::createPolicy,
        py::arg("input"), py::arg("repos"), py::arg("validate")=true);
    clsPolicy.def_static("createPolicy", (Policy* (*)(const std::string&, const std::string&, bool)) &Policy::createPolicy,
        py::arg("input"), py::arg("repos"), py::arg("validate")=true);
    clsPolicy.def("getValueType", (Policy::ValueType (Policy::*)(const std::string&) const) &Policy::getValueType);
    clsPolicy.def("nameCount", &Policy::nameCount);
    clsPolicy.def("names", (int (Policy::*)(std::list<std::string>&, bool, bool) const) &Policy::names,
        py::arg("names"), py::arg("topLevelOnly")=false, py::arg("append")=false);
    clsPolicy.def("paramNames", (int (Policy::*)(std::list<std::string>&, bool, bool) const) &Policy::paramNames,
        py::arg("names"), py::arg("topLevelOnly")=false, py::arg("append")=false);
    clsPolicy.def("policyNames", (int (Policy::*)(std::list<std::string>&, bool, bool) const) &Policy::policyNames,
        py::arg("names"), py::arg("topLevelOnly")=false, py::arg("append")=false);
    clsPolicy.def("fileNames", (int (Policy::*)(std::list<std::string>&, bool, bool) const) &Policy::fileNames,
        py::arg("names"), py::arg("topLevelOnly")=false, py::arg("append")=false);
    clsPolicy.def("names", (Policy::StringArray (Policy::*)(bool) const) &Policy::names, py::arg("topLevelOnly")=false);
    clsPolicy.def("paramNames", (Policy::StringArray (Policy::*)(bool) const) &Policy::paramNames, py::arg("topLevelOnly")=false);
    clsPolicy.def("policyNames", (Policy::StringArray (Policy::*)(bool) const) &Policy::policyNames, py::arg("topLevelOnly")=false);
    clsPolicy.def("fileNames", (Policy::StringArray (Policy::*)(bool) const) &Policy::fileNames, py::arg("topLevelOnly")=false);
    clsPolicy.def("isDictionary", &Policy::isDictionary);
    clsPolicy.def("canValidate", &Policy::canValidate);
    clsPolicy.def("getDictionary", &Policy::getDictionary);
    clsPolicy.def("setDictionary", &Policy::setDictionary);
    // Somehow default arguments don't work here
    clsPolicy.def("validate", [](Policy& p){
        return p.validate();
    });
    clsPolicy.def("validate", [](Policy& p, ValidationError *errs){
        return p.validate(errs);
    });
    clsPolicy.def("valueCount", &Policy::valueCount);
    clsPolicy.def("isArray", &Policy::isArray);
    clsPolicy.def("exists", &Policy::exists);
    clsPolicy.def("isBool", &Policy::isBool);
    clsPolicy.def("isInt", &Policy::isInt);
    clsPolicy.def("isDouble", &Policy::isDouble);
    clsPolicy.def("isString", &Policy::isString);
    clsPolicy.def("isPolicy", &Policy::isPolicy);
    clsPolicy.def("isFile", &Policy::isFile);
    clsPolicy.def("getTypeInfo", &Policy::getTypeInfo);
    clsPolicy.def("getPolicy", (Policy::Ptr (Policy::*)(const std::string&)) &Policy::getPolicy);
    clsPolicy.def("getFile", &Policy::getFile);
    clsPolicy.def("getBool", &Policy::getBool);
    clsPolicy.def("getInt", &Policy::getInt);
    clsPolicy.def("getDouble", &Policy::getDouble);
    clsPolicy.def("getString", &Policy::getString);
    clsPolicy.def("getPolicyArray", &Policy::getPolicyArray);
    clsPolicy.def("getFileArray", &Policy::getFileArray);
    clsPolicy.def("getBoolArray", &Policy::getBoolArray);
    clsPolicy.def("getIntArray", &Policy::getIntArray);
    clsPolicy.def("getDoubleArray", &Policy::getDoubleArray);
    clsPolicy.def("getStringArray", &Policy::getStringArray);
    clsPolicy.def("set", (void (Policy::*)(const std::string&, const Policy::Ptr&)) &Policy::set);
    clsPolicy.def("set", (void (Policy::*)(const std::string&, bool)) &Policy::set);
    clsPolicy.def("set", (void (Policy::*)(const std::string&, int)) &Policy::set);
    clsPolicy.def("set", (void (Policy::*)(const std::string&, double)) &Policy::set);
    clsPolicy.def("set", (void (Policy::*)(const std::string&, const std::string&)) &Policy::set);
    clsPolicy.def("add", (void (Policy::*)(const std::string&, const Policy::Ptr&)) &Policy::add);
    clsPolicy.def("add", (void (Policy::*)(const std::string&, bool)) &Policy::add);
    clsPolicy.def("add", (void (Policy::*)(const std::string&, int)) &Policy::add);
    clsPolicy.def("add", (void (Policy::*)(const std::string&, double)) &Policy::add);
    clsPolicy.def("add", (void (Policy::*)(const std::string&, const std::string&)) &Policy::add);
    clsPolicy.def("remove", &Policy::remove);
    clsPolicy.def("loadPolicyFiles", (int (Policy::*)(bool)) &Policy::loadPolicyFiles, py::arg("strict")=true);

    // boost::filesystem::path is equivalent to str in Python
    clsPolicy.def("loadPolicyFiles", [](Policy &p, const std::string& path, bool strict=true) -> int {
            return p.loadPolicyFiles(boost::filesystem::path(path), strict);
        }, py::arg("repository"), py::arg("strict")=true);

    clsPolicy.def("mergeDefaults", &Policy::mergeDefaults, py::arg("defaultPol"), py::arg("keepForValidation")=true,
        py::arg("errs")=nullptr);
    clsPolicy.def("str", &Policy::str, py::arg("name"), py::arg("indent")="");
    clsPolicy.def("toString", &Policy::toString);
    clsPolicy.def("asPropertySet", &Policy::asPropertySet);

    return mod.ptr();
}
