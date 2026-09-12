#include <pybind11/pybind11.h>
#include "Path.hpp"
#include "loc.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_pc, m) {

    py::class_<PathTypeHint> _path_hint(m, "Path", py::module_local());
    _path_hint.attr("__module__") = py::cast(".Path");

    py::class_<_Path>(m, "_Path")
        .def(py::init<py::object>())
        .def_static("_parse", &_Path::_parse)
        .def("__str__", &_Path::__str__)
        .def("__repr__", &_Path::__str__)
        .def_readonly("path", &_Path::path)
        .def_readonly("wpath", &_Path::wpath)
        .def_readonly("_pure", &_Path::_pure)
        .def_property_readonly("ext", &_Path::ext)
        .def_property_readonly("name", &_Path::name)
        .def_property_readonly("exists", &_Path::exists)
        .def_property_readonly("is_file", &_Path::is_file)
        .def_property_readonly("is_dir", &_Path::is_dir);

    py::class_<_loc>(m, "_loc")
        .def(py::init<>())
        .def_property_readonly("temp", &_loc::get_temp)
        .def_property_readonly("script", &_loc::get_script)
        .def_property_readonly("cache", &_loc::get_cache);
    
    m.attr("loc") = py::cast(_loc());

}
