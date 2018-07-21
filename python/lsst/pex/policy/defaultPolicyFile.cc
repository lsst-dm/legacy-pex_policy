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

#include "lsst/pex/policy/DefaultPolicyFile.h"
#include "lsst/pex/policy/PolicyFile.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace pex {
namespace policy {

PYBIND11_MODULE(defaultPolicyFile, mod) {
    py::class_<DefaultPolicyFile, std::shared_ptr<DefaultPolicyFile>, PolicyFile> cls(mod,
                                                                                      "DefaultPolicyFile");

    cls.def(py::init<const char* const, const std::string&, const std::string&, bool>(), "productName"_a,
            "filepath"_a, "repos"_a = "", "strict"_a = true);

    cls.def("load", &DefaultPolicyFile::load);
    cls.def("getRepositoryPath",
            [](DefaultPolicyFile const& self) -> std::string { return self.getRepositoryPath().native(); });
}

}  // policy
}  // pex
}  // lsst
