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

#include "lsst/pex/policy/PolicyFile.h"
#include "lsst/pex/policy/Dictionary.h"

using namespace lsst::pex::policy;

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_policyFile) {
    py::module mod("_policyFile", "Access to the classes from the pex policy PolicyFile library");

    py::class_<PolicyFile, std::shared_ptr<PolicyFile>> cls(mod, "PolicyFile", py::base<PolicySource>());

    // SupportedFormats is not exposed to Python so don't export the default argument
    cls.def("__init__", [](PolicyFile &p, const std::string &filepath) {
        new (&p) PolicyFile(filepath);
    });

    cls.def("getPath", &PolicyFile::getPath);
    cls.def("exists", &PolicyFile::exists);
    cls.def("getFormatName", (const std::string& (PolicyFile::*)()) &PolicyFile::getFormatName);
    cls.def("load", (void (PolicyFile::*)(Policy&)) &PolicyFile::load);

    cls.def_readonly_static("EXT_PAF", &PolicyFile::EXT_PAF);
    cls.def_readonly_static("EXT_XML", &PolicyFile::EXT_XML);

    return mod.ptr();
}
