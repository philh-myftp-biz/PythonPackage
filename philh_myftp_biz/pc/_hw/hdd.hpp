#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <array>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <memory>
#include <functional>
#include <pybind11/stl.h>

#include <subprocess.hpp>
#include <json.hpp>
#include <hwinfo/hwinfo.h>
#include <stru.h>
#include <remap.h>

#include <pc/_hw/device.h>

struct HardDrive : public Device {

    static vector<HardDrive> search() {
        vector<HardDrive> _hdds;

        for (const auto& dsk : hwinfo::getAllDisksClean()) {
            _hdds.push_back(HardDrive(
                "?", // Tower
                "?", // COnn
                -1, // Slot
                dsk.serial_number() // SN
            ));
        }

        return _hdds;
    }

    //===============================================================================
    // Init

    str Tower;
    str Conn;
    str SN;
    int Slot;

    void* cached_hDevInfo = nullptr; 

    HardDrive(
        str Tower,
        str Conn,
        int Slot,
        str SN
    ) {
        this->Tower = Tower;
        this->Conn = Conn;
        this->Slot = Slot;
        this->SN = py::bytes(SN).attr("decode")("latin1").cast<str>();
    }

    //===============================================================================
    // Windows Helpers
    #ifdef WINDOWS

        json _powershell(str cmd) const {
            auto [exit_code, out_buf, err_buf] = sp::capture_run(
                narg::$powershell,
                "Get-PhysicalDisk -SerialNumber '" + SN + "' | Select -First 1 " + cmd + " | ConvertTo-Json"
            );
            return (exit_code == 0 && !out_buf.empty()) 
                ? json::parse( out_buf.to_string() )
                : json::object();
        }

    #endif
    //===============================================================================
    // hwDisk

    using optDisk = optional<hwinfo::Disk>;

    mutable optDisk _cached_disk = nullopt; 

    optDisk hwDisk() const {
        if (!_cached_disk.has_value()) {
            for (const auto& dsk : hwinfo::getAllDisksClean()) {
                if (stru::match_str_nc(this->SN, dsk.serial_number())) {
                    _cached_disk = dsk;
                    break;
                }
            }
        }
        return _cached_disk;
    }

    //===============================================================================
    // ID

    int GetID() const override {
        size_t id = std::hash<str>{}(SN);
        return static_cast<int>(id);
    }

    //===============================================================================
    // Name

    str GetName() const override {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << Slot << "-" << Tower;
        oss << " [" << Index() << ", " << SN << "]";
        return oss.str();
    }

    //===============================================================================
    // Connected

    bool GetConnected() const override {
        return hwDisk().has_value();
    }

    //===============================================================================
    // Index

    int Index() const {
        return GetConnected() ? hwDisk()->id() : -1;
    }

    //===============================================================================
    // FriendlyName

    str GetFriendlyName() const override {
        if (!GetConnected()) return "";

        str model = hwDisk().has_value() ? hwDisk()->model() : "";

        #ifdef WINDOWS
            json data = _powershell("");
            return data.value("FriendlyName", model);
        #endif

        return model;        
    }

    void setFriendlyName(str name) {
        if (!GetConnected()) return;
        #ifdef WINDOWS
            _powershell("| Set-PhysicalDisk -NewFriendlyName '" + name + "'");
        #endif
    }

    //===============================================================================
    // Usage

    str Usage() {
        if (!GetConnected()) return "";

        #ifdef WINDOWS
            json data = _powershell(""); 
            return data.value("Usage", "");
        #endif

        return "";
    }

    void setUsage(str usage) {
        if (!GetConnected()) return;
        #ifdef WINDOWS
            _powershell("Set-PhysicalDisk -Usage '" + usage + "'");
        #endif
    }

    //===============================================================================
    // HealthReport

    str GetHealthReport() const override {

        str SerialNumber = "N/A";
        str Model = "N/A";
        str Status = "Missing";
        
        if (GetConnected()) {
            SerialNumber = SN;
            Model = hwDisk()->model();
            switch (GetHealthStatus()) {
                case HEALTHY: Status = "Healthy";
                case WARNING: Status = "Warning";
                case NOTGOOD: Status = "NotGood";
                default: Status = "Unknown";
            }
        }

        std::ostringstream msg;
        msg << "SERIAL NUMBER: " << SerialNumber << "\n";
        msg << "MODEL........: " << Model << "\n";
        msg << "STATUS.......: " << Status << "\n";

        return msg.str();
    }

    //===============================================================================
    // Health

    HealthStatus GetHealthStatus() const override {
        if (!GetConnected()) return MISSING;

        #ifdef WINDOWS
            json data = _powershell("");
            std::string status = data.value("HealthStatus", "Unknown");
            if (status == "Healthy")   return HEALTHY;
            if (status == "Warning")   return WARNING;
            if (status == "Unhealthy") return NOTGOOD;
            return UNKNOWN;
        #else
            auto [exit_code, out_buf, err_buf] = sp::capture_run("smartctl", {
                "-H", 
                ("/dev/sd" + str(1, 'a' + hwDisk()->id()))
            });
            return (exit_code == 0 && out_buf.to_string().find("PASSED") != str::npos) 
                ? HEALTHY 
                : NOTGOOD;
        #endif
    }
    
    //===============================================================================

};
