#include <pybind11/pybind11.h>
#include "FirewallException.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_web, m) {

    py::class_<FirewallException>(m, "FirewallException")
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("__repr__", &FirewallException::repr)            
        .def("delete", &FirewallException::delete_rule)
        .def_property_readonly("exists", &FirewallException::exists)
        .def("set", &FirewallException::set, 
            py::arg("port"), 
            py::arg("dir") = "in"
        );

}
