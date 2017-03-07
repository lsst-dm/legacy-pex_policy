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

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace pex {
namespace policy {

PYBIND11_PLUGIN(urnPolicyFile) {
    py::module mod("urnPolicyFile");

    py::class_<UrnPolicyFile, std::shared_ptr<UrnPolicyFile>, DefaultPolicyFile> cls(mod, "UrnPolicyFile");

    cls.def(py::init<const std::string&, bool, bool>(), "urn"_a, "strictUrn"_a = false,
            "strictLoads"_a = true);

    cls.def_static("productNameFromUrn", &UrnPolicyFile::productNameFromUrn);
    cls.def_static("filePathFromUrn", &UrnPolicyFile::filePathFromUrn);
    cls.def_static("reposFromUrn", &UrnPolicyFile::reposFromUrn);
    cls.def_readonly_static("URN_PREFIX", &UrnPolicyFile::URN_PREFIX);
    cls.def_readonly_static("URN_PREFIX_ABBREV", &UrnPolicyFile::URN_PREFIX_ABBREV);
    cls.def_static("looksLikeUrn", &UrnPolicyFile::looksLikeUrn);

    return mod.ptr();
}

}  // policy
}  // pex
}  // lsst
