#pragma once

#include <string>
#include <functional>
#include <pybind11/pybind11.h>

#include <json.hpp>
#include "remap.h"

class Device { public:

    static vector<Device> search() {
        vector<Device> devs;
        return devs;
    }

    virtual ~Device() = default;
    
    virtual str GetName() const {
        return "";
    }

    virtual str GetHealthReport() const {
        return "";
    }

    virtual str GetLink() const {
        return "";
    }

    virtual str GetFriendlyName() const {
        return "";
    }

    virtual bool GetConnected() const {
        return false;
    }

    virtual int GetID() const {
        json j;
        j["name"] = GetName();
        j["health"] = GetHealthReport();
        j["connected"] = GetConnected();

        size_t id = std::hash<str>{}(j.dump());
        return static_cast<int>(id);
    }

};
