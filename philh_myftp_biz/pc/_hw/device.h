#pragma once

#include <string>
#include <pybind11/pybind11.h>

#include "remap.h"

namespace py = pybind11;

// 1. The Strict Superclass (Abstract Base Class)
class Device { public:

    virtual ~Device() = default;

    // Pure virtual functions make this class abstract and strictly for inheritance
    virtual str GetName() const = 0;
    virtual str GetHealthReport() const = 0;
    virtual bool GetConnected() const = 0;

};

// 2. The Trampoline Class (Allows Python to inherit from Device)
class PyDevice : public Device { public:

    using Device::Device;

    // Directs pure virtual lookups back to Python
    str GetName() const override {
        PYBIND11_OVERRIDE_PURE(str, Device, GetName);
    }

    str GetHealthReport() const override {
        PYBIND11_OVERRIDE_PURE(str, Device, GetHealthReport);
    }

    bool GetConnected() const override {
        PYBIND11_OVERRIDE_PURE(bool, Device, GetConnected);
    }

};
