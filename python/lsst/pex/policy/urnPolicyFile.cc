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

#include "lsst/pex/policy/UrnPolicyFile.h"
#include "lsst/pex/policy/DefaultPolicyFile.h"

using namespace lsst::pex::policy;

namespace py = pybind11;

PYBIND11_PLUGIN(_urnPolicyFile) {
    py::module mod("_urnPolicyFile", "Access to the classes from the pex policy UrnPolicyFile library");

    py::class_<UrnPolicyFile> cls(mod, "UrnPolicyFile", py::base<DefaultPolicyFile>());

    cls.def(py::init<const std::string&, bool, bool>(),
        py::arg("urn"), py::arg("strictUrn")=false, py::arg("strictLoads")=true);

    cls.def_static("productNameFromUrn", &UrnPolicyFile::productNameFromUrn);
    cls.def_static("filePathFromUrn", &UrnPolicyFile::filePathFromUrn);
    cls.def_static("reposFromUrn", &UrnPolicyFile::reposFromUrn);
    cls.def_readonly_static("URN_PREFIX", &UrnPolicyFile::URN_PREFIX);
    cls.def_readonly_static("URN_PREFIX_ABBREV", &UrnPolicyFile::URN_PREFIX_ABBREV);
    cls.def_static("looksLikeUrn", &UrnPolicyFile::looksLikeUrn);

    return mod.ptr();
}
