#pragma once
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <iostream>
#include <chrono>
#include <string>
#include "remap.h"

class TimeStamp { public:

    int year, month, day, hour, minute, second, decisecond, centisecond, millisecond, microsecond;
    str iso_format;
    py::object dt;
    double unix;

    inline static const str default_tz = "America/New_York";

    TimeStamp(double stamp, str tz=default_tz) {

        this->unix = stamp;

        py::object dt = import("datetime").attr("datetime").attr("fromtimestamp") (
            unix,
            py::arg("tz") = import("pytz").attr("timezone")(tz)
        );

        year = dt.attr("year").cast<int>();
        month = dt.attr("month").cast<int>();
        day = dt.attr("day").cast<int>();
        hour = dt.attr("hour").cast<int>();
        minute = dt.attr("minute").cast<int>();
        second = dt.attr("second").cast<int>();

        microsecond = dt.attr("microsecond").cast<int>();
        decisecond  = microsecond / 100000;
        centisecond = microsecond / 10000;
        millisecond = microsecond / 1000;

        iso_format = dt.attr("isoformat")().cast<str>();

    }

    py::object stamp(str format) {
        return dt.attr("strftime") (format);        
    }

    static double _parse(py::object other) {

        if (py::isinstance<TimeStamp>(other))
            return other.cast<TimeStamp&>().unix;

        if (py::isinstance<py::int_>(other) || py::isinstance<py::float_>(other))
            return other.cast<double>();

        return -1;
    }

};
