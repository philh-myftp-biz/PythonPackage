#pragma once
#include <string>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// 1. The Strict Superclass (Abstract Base Class)
class Device {
public:
    virtual ~Device() = default;

    // Pure virtual functions make this class abstract and strictly for inheritance
    virtual std::string GetName() const = 0;
    virtual bool GetConnected() const = 0;
};

// 2. The Trampoline Class (Allows Python to inherit from Device)
class PyDevice : public Device {
public:
    using Device::Device;

    // Directs pure virtual lookups back to Python
    std::string GetName() const override {
        PYBIND11_OVERRIDE_PURE(std::string, Device, GetName);
    }

    bool GetConnected() const override {
        PYBIND11_OVERRIDE_PURE(bool, Device, GetConnected);
    }
};
