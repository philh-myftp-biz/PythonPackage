#pragma once
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include "remap.h"

#include <chrono>
#include <string>

class Stopwatch { public:

    double start_time = -1.0;
    double end_time = -1.0;
    bool running = false;

    static double _now() {
        auto now = chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return chrono::duration<double>(duration).count();
    }

    double elapsed() {
        if (start_time == -1.0) {
            return -1.0;
        } else if (running) {
            return _now() - start_time;
        } else {
            return end_time - start_time;
        }
    }

    Stopwatch& start() {
        start_time = _now();
        end_time = -1.0;
        running = true;
        return *this;
    }

    Stopwatch& stop() {
        end_time = _now();
        running = false;
        return *this;
    }

};
