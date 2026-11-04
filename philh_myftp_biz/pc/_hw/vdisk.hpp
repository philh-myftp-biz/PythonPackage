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
        this->_ps_append = "-VirtualDisk -FriendlyName '" + Name + " | Select -First 1";
    }

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
        if (connected) {
            // Connect-VirtualDisk
            _powershell("Connect");
        } else {
            // Disconnect-VirtualDisk
            _powershell("Disconnect");
        }
    }

    //===============================================================================
    // Repair

    void Repair() {
        // Repair-VirtualDisk
        _powershell("Repair");
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

        json data = _powershell("Get"); // Get-VirtualDisk
        str status = data.value("HealthStatus", "Unknown");
        if (status == "Healthy")   return HEALTHY;
        if (status == "Warning")   return WARNING;
        if (status == "Unhealthy") return NOTGOOD;
        return UNKNOWN;
    }
    
    //===============================================================================

};
