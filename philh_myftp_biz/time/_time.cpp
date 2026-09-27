#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include "remap.h"

#include <stopwatch.hpp>
#include <timeout.hpp>
#include <timestamp.hpp>

PYBIND11_MODULE(_time, m) {

    // Stopwatch Binding
    py::class_<Stopwatch>(m, "Stopwatch")
        .def(py::init<>())
        .def("start", &Stopwatch::start, py::return_value_policy::reference_internal)
        .def("stop", &Stopwatch::stop, py::return_value_policy::reference_internal)
        .def("__int__", [](Stopwatch& self) { return static_cast<int>(self.elapsed()); })
        .def("__float__", &Stopwatch::elapsed)
        .def("__gt__", [](Stopwatch& self, double val) { return self.elapsed() >  val; })
        .def("__ge__", [](Stopwatch& self, double val) { return self.elapsed() >= val; })
        .def("__lt__", [](Stopwatch& self, double val) { return self.elapsed() <  val; })
        .def("__le__", [](Stopwatch& self, double val) { return self.elapsed() <= val; })
        .def("__eq__", [](Stopwatch& self, double val) { return self.elapsed() == val; })
        .def_property_readonly("elapsed", &Stopwatch::elapsed)
        .def_readonly("running", &Stopwatch::running);

    // Timeout Binding
    py::class_<Timeout, Stopwatch>(m, "Timeout")
        .def(py::init<int>(),
            py::arg("timeout")
        )
        .def_property_readonly("timed_out", &Timeout::timed_out)
        .def("check", &Timeout::check);

    // TimeStamp Binding
    py::class_<TimeStamp>(m, "TimeStamp")
        .def(py::init<double, const str&>(),
            py::arg("stamp"),
            py::arg("tz") = TimeStamp::default_tz
        )
        .def("stamp", &TimeStamp::stamp,
            py::arg("format") = "%Y-%m-%d %H:%M:%S"
        )
        .def_readonly("year", &TimeStamp::year)
        .def_readonly("month", &TimeStamp::month)
        .def_readonly("day", &TimeStamp::day)
        .def_readonly("hour", &TimeStamp::hour)
        .def_readonly("minute", &TimeStamp::minute)
        .def_readonly("second", &TimeStamp::second)
        .def_readonly("decisecond", &TimeStamp::decisecond)
        .def_readonly("centisecond", &TimeStamp::centisecond)
        .def_readonly("millisecond", &TimeStamp::millisecond)
        .def_readonly("microsecond", &TimeStamp::microsecond)
        .def_readonly("unix", &TimeStamp::unix)
        .def_readonly("ISO", &TimeStamp::iso_format)
        .def("__float__", [](TimeStamp& self) { return self.unix; })
        .def("__int__", [](TimeStamp& self) { 
            return static_cast<int>(self.unix); 
        })
        .def("__repr__", [](TimeStamp& self) {
            return "<TimeStamp '" + self.iso_format + ">";
        })
        .def("__eq__", [](TimeStamp& self, py::object other) {
            return self.unix == self._parse(other);
        })
        .def("__lt__", [](TimeStamp& self, py::object other) {
            return self.unix < self._parse(other);
        })
        .def("__gt__", [](TimeStamp& self, py::object other) {
            return self.unix > self._parse(other);
        });

}
