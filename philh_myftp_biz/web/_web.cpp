#include <pybind11/pybind11.h>

#include "remap.h"

#include "FirewallException.hpp"
#include "IP.hpp"

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

    py::class_<_IP>(m, "_IP")
        .def_property_readonly("LAN", &_IP::LAN)
        .def_property_readonly("WAN", &_IP::WAN);
    
    m.attr("IP") = py::cast(_IP());

}
