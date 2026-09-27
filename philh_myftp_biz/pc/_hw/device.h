#pragma once

#include <string>
#include <pybind11/pybind11.h>

#include "remap.h"

class Device { public:

    virtual ~Device() = default;
    
    virtual str GetName() const {
        return "";
    }

    virtual str GetHealthReport() const {
        return "";
    }

    virtual bool GetConnected() const {
        return false;
    }

};
