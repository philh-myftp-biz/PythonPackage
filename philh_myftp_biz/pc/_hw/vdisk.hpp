#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#include <subprocess.hpp>
#include <json.hpp>
#include <remap.h>

#include <pc/_hw/device.h>

struct VirtualDisk : public Device {

    str Name;
    str Mount;

    str GetName() const override { return this->Name; }

    //===============================================================================

    VirtualDisk(
        str Name,
        str Mount
    ) {
        this->Name = Name;
        this->Mount = Mount;
    }

    //===============================================================================
    // Windows Helpers
    #ifdef WINDOWS

        json _powershell(str cmd) const {
            auto [exit_code, out_buf, err_buf] = sp::capture_run(
                narg::$powershell,
                cmd + "-VirtualDisk -FriendlyName '" + Name + "' | Select -First 1 | ConvertTo-Json" 
            );
            return (exit_code == 0 && !out_buf.empty()) 
                ? json::parse( out_buf.to_string() )
                : json::object();
        }
    
    #endif
    //===============================================================================
    // ID

    int GetID() const override {
        size_t hash = std::hash<str>{}(Mount);
        return static_cast<int>(hash);
    }
    
    //===============================================================================
    // Connected

    bool GetConnected() const override {
        std::error_code ec;
        return fs::exists(Mount, ec) && !ec;
    }

    void setConnected(bool connected) {
        #ifdef WINDOWS
        if (connected) {
            // Connect-VirtualDisk
            _powershell("Connect");
        } else {
            // Disconnect-VirtualDisk
            _powershell("Disconnect");
        }
        #endif
    }

    //===============================================================================
    // Repair

    void Repair() {
        #ifdef WINDOWS
        // Repair-VirtualDisk
        _powershell("Repair");
        #endif
    }

    //===============================================================================
    // HealthReport

    str GetHealthReport() const override {

        str Status = "Missing";
        if (GetConnected()) {
            switch (GetHealthStatus()) {
                case HEALTHY: Status = "Healthy";
                case WARNING: Status = "Warning";
                case NOTGOOD: Status = "NotGood";
                default: Status = "Unknown";
            }
        }

        std::ostringstream msg;
        msg << "NAME..: " << Name << "\n";
        msg << "MOUNT.: " << Mount << "\n";
        msg << "STATUS: " << Status << "\n";

        return msg.str();
    }

    //===============================================================================
    // Health

    HealthStatus GetHealthStatus() const override {
        if (!GetConnected()) return MISSING;

        #ifdef WINDOWS
            json data = _powershell("Get"); // Get-VirtualDisk
            str status = data.value("HealthStatus", "Unknown");
            if (status == "Healthy")   return HEALTHY;
            if (status == "Warning")   return WARNING;
            if (status == "Unhealthy") return NOTGOOD;
            return UNKNOWN;
        #else
            return HEALTHY;
        #endif
    }
    
    //===============================================================================

};
