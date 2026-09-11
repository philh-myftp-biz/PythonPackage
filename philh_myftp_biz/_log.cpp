#include <pybind11/pybind11.h>
#include "Formatter.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_log, m) {

    py::class_<Formatter>(m, "Formatter")
        .def(py::init<>())
        .def("format", &Formatter::format);

}
