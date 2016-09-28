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

#include "lsst/pex/policy/SupportedFormats.h"

using namespace lsst::pex::policy;

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_supportedFormats) {
    py::module mod("_supportedFormats", "Access to the classes from the pex policy SupportedFormats library");

    py::class_<SupportedFormats, std::shared_ptr<SupportedFormats>> cls(mod, "SupportedFormats");

    cls.def(py::init<>());

    cls.def("registerFormat", &SupportedFormats::registerFormat);
    cls.def("recognizeType", &SupportedFormats::recognizeType);
    cls.def("supports", &SupportedFormats::supports);
    cls.def("size", &SupportedFormats::size);

    return mod.ptr();
}
