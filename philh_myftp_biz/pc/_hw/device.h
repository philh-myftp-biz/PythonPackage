#pragma once

#include <string>
#include <functional>
#include <optional>
#include <pybind11/pybind11.h>

#include <json.hpp>
#include "remap.h"

enum HealthStatus {
    UNKNOWN, // 0
    HEALTHY, // 1
    WARNING, // 2
    NOTGOOD, // 3
    MISSING, // 4
    RUNNING, // 5
    STOPPED, // 6
};

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

    virtual HealthStatus GetHealthStatus() const {
        return GetConnected() ? HEALTHY : MISSING;
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

    virtual optional<float> GetTemperature() const {
        return nullopt;
    }

    str _ps_prepend = "";
    str _ps_append = "";

    #ifdef WINDOWS

        json _powershell(str cmd="") const {
            auto [exit_code, out_buf, err_buf] = sp::capture_run(
                narg::$powershell,
                (_ps_prepend + cmd + _ps_append + " | ConvertTo-Json")
            );
            return (exit_code == 0 && !out_buf.empty()) 
                ? json::parse( out_buf.to_string() )
                : json::object();
        }
    
    #else

        json _powershell(str cmd="") const {
            return json::object();
        }
    
    #endif

};
