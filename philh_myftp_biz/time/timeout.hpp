#pragma once
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <chrono>
#include <string>
#include "remap.h"

#include <stopwatch.hpp>

class Timeout : public Stopwatch { public:
 
    double timeout;
    str message;
    
    Timeout(double timeout): Stopwatch() {
        this->timeout = timeout;
        start();
    }

    bool timed_out() {
        if (elapsed() != -1.0)
            return elapsed() >= timeout;
        return false;
    }

    void check() {
        if (timed_out()) {
            py::set_error(PyExc_TimeoutError, "The operation timed out.");
            throw py::error_already_set();
        }
    }

};

