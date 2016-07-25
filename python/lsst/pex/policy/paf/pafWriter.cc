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

#include "lsst/pex/policy/paf/PAFWriter.h"
#include "lsst/pex/policy/Policy.h"

using namespace lsst::pex::policy::paf;

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_pafWriter) {
    py::module mod("_pafWriter", "Access to the classes from the pex policy PAFWriter library");

    py::class_<PAFWriter> cls(mod, "PAFWriter");

    cls.def(py::init<>());
    cls.def(py::init<const std::string&>());
    
    cls.def("writeBools", &PAFWriter::writeBools);
    cls.def("writeInts", &PAFWriter::writeInts);
    cls.def("writeDoubles", &PAFWriter::writeDoubles);
    cls.def("writeStrings", &PAFWriter::writeStrings);
    cls.def("writePolicies", &PAFWriter::writePolicies);
    cls.def("writeFiles", &PAFWriter::writeFiles);

    /* Inherited from PolicyWriter */
    cls.def("write", (void (PAFWriter::*)(const lsst::pex::policy::Policy&, bool)) &PAFWriter::write,
        py::arg("policy"), py::arg("doDecl")=false);
    cls.def("close", (void (PAFWriter::*)()) &PAFWriter::close);
    cls.def("toString", (std::string (PAFWriter::*)()) &PAFWriter::toString);

    return mod.ptr();
}
