#include <pybind11/pybind11.h>
#include "Formatter.hpp"
#include "VERBOSE.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_log, m) {

    py::class_<Formatter>(m, "Formatter")
        .def(py::init<>())
        .def("format", &Formatter::format);

    py::class_<MutInt>(m, "MutInt")
        .def(py::init<int>())
        .def_readwrite("value", &MutInt::value)
        .def("__bool__", &MutInt::__bool__)
        .def("__int__", &MutInt::__int__)
        .def("__index__", &MutInt::__int__)
        .def("__str__", &MutInt::__str__)
        .def("__repr__", &MutInt::__str__)
        .def("__iadd__", &MutInt::__iadd__, py::is_operator())
        .def("__isub__", &MutInt::__isub__, py::is_operator())
        .def("__add__", &MutInt::__add__, py::is_operator())
        .def("__sub__", &MutInt::__sub__, py::is_operator());

    py::class_<VERBOSE, MutInt>(m, "VERBOSE")
        .def(py::init<int>(), py::arg("parsed_lvalue"))
        .def_readwrite("lvalue", &VERBOSE::lvalue)
        .def("pause", &VERBOSE::pause)
        .def("resume", &VERBOSE::resume)
        .def("enable", &VERBOSE::enable)
        .def("disable", &VERBOSE::disable);

}
