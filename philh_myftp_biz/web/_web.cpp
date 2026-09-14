#include <pybind11/pybind11.h>
#include "FirewallException.hpp"
#include "IP.hpp"
#include "URL.hpp"

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

    py::class_<_IP>(m, "_IP")
        .def_property_readonly("LAN", &_IP::LAN)
        .def_property_readonly("WAN", &_IP::WAN);
    m.attr("IP") = py::cast(_IP());

    py::class_<URL>(m, "URL")
        .def(
            py::init<std::string, std::map<std::string, std::string>, std::map<std::string, std::string>, int, int, int>(),
            py::arg("url"),
            py::arg("params") = std::map<std::string, std::string>{},
            py::arg("headers") = std::map<std::string, std::string>{},
            py::arg("max_tries") = 1,
            py::arg("max_age") = 0,
            py::arg("timeout") = 30
        )
        .def("copy", &URL::copy)
        .def("child", &URL::child, py::arg("name"))
        .def("download", &URL::download, py::arg("path"), py::arg("force") = true)
        .def("format", &URL::format)
        .def("__str__", &URL::to_string)
        .def("__repr__", &URL::to_string)
        .def_readonly("url", &URL::base_url)
        .def_readonly("params", &URL::params)
        .def_readonly("headers", &URL::headers)
        .def_readonly("timeout", &URL::timeout)
        .def_readonly("addr", &URL::host)
        .def_property_readonly("furl", &URL::to_string)
        .def_property_readonly("text", &URL::text)
        .def_property_readonly("json", &URL::json)
        .def_property_readonly("exists", &URL::exists)
        .def_property_readonly("size", &URL::size);

}
