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
#include <pybind11/stl.h>

#include <subprocess.hpp>
#include <json.hpp>
#include <hwinfo/hwinfo.h>
#include <stru.h>
#include <remap.h>

#include <_hw/device.h>

struct HardDrive : public Device {

    static std::vector<HardDrive> search() {
        std::vector<HardDrive> _hdds;

        for (const auto& _hwDisk : hwinfo::getAllDisks()) {
            
            str _sn = stru::strip( _hwDisk.serial_number() );
            if (_sn.starts_with('{')) continue;
            if (_sn.empty()) continue;
            
            _hdds.push_back(HardDrive(
                "?", // Tower
                "?", // COnn
                -1, // ID
                _sn // SN
            ));
            
        }

        return _hdds;
    }

    //===============================================================================
    // Init

    str Tower;
    str Conn;
    str SN;
    int ID;

    void* cached_hDevInfo = nullptr; 

    HardDrive(
        str Tower,
        str Conn,
        int ID,
        str SN
    ) {
        this->Tower = Tower;
        this->Conn = Conn;
        this->ID = ID;
        this->SN = SN;
    }

    //===============================================================================
    // Windows Helpers
    #ifdef WINDOWS

        str _powershell(str cmd) const {
            str script = "Get-PhysicalDisk | Where-Object SerialNumber -eq '" + SN + "' | " + cmd;
            
            auto [exit_code, out_buf, err_buf] = subprocess::capture_run(narg::$powershell, script);
            
            if (exit_code == 0) {
                return out_buf.to_string();
            } else {
                return "";
            }
        }

    #endif
    //===============================================================================
    // hwDisk

    mutable optional<hwinfo::Disk> _cached_hwDisk = nullopt;

    optional<hwinfo::Disk> hwDisk() const {
        using namespace stru;

        if (!_cached_hwDisk.has_value()) {
            for (const auto& _hwDisk : hwinfo::getAllDisks()) {
                
                str raw_sn = strip(_hwDisk.serial_number());

                if (!raw_sn.empty() && match_str_nc(this->SN, raw_sn)) {
                    _cached_hwDisk = _hwDisk;
                    break;
                }

            }
        }
        
        return _cached_hwDisk;
    }

    //===============================================================================
    // Name

    str GetName() const override {
        int _index = const_cast<HardDrive*>(this)->Index(); 
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << ID << "-" << Tower;
        oss << " [" << _index << ", " << SN << "]";
        return oss.str();
    }

    //===============================================================================
    // Connected

    bool GetConnected() const override {
        return hwDisk().has_value();
    }

    //===============================================================================
    // Index

    int Index() {
        if (GetConnected()) {
            return hwDisk()->id();
        } else {
            return -1;
        }
    }

    //===============================================================================
    // FriendlyName

    str FriendlyName() const {
        if (!GetConnected()) return "";

        #ifdef WINDOWS
            str jsonRaw = _powershell("Select-Object -Property FriendlyName | ConvertTo-Json");
            json data = json::parse(jsonRaw);
            if (data.contains("FriendlyName") && !data["FriendlyName"].is_null())
                return data["FriendlyName"].get<str>();
        #endif
        
        return hwDisk().has_value() ? hwDisk()->model() : "";
    }

    void setFriendlyName(str name) {
        if (!GetConnected()) return;
        #ifdef WINDOWS
            _powershell("Set-PhysicalDisk -NewFriendlyName '" + name + "'");
        #endif
    }

    //===============================================================================
    // Usage

    str Usage() {
        if (!GetConnected()) return "";

        #ifdef WINDOWS
            str raw = _powershell("Select-Object -Property Usage | ConvertTo-Json"); 
            json data = json::parse(raw);
            if (data.contains("Usage"))
                return data["Usage"].get<str>();
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
        
        if (!GetConnected()) {
            return "DISK DETECT STATUS... [ NOT FOUND ]\n"
                   "CRITICAL: DEVICE DISCONNECTED OR NOT RESPONDING.";
        }

        std::ostringstream msg;
        msg << "PRI. MASTER DISK: " << hwDisk()->model() << "\n";
        msg << "SERIAL NUMBER   : " << SN << "\n";
        msg << "S.M.A.R.T. CAPABLE AND STATUS CHECKING... ";

        #ifdef WINDOWS
            // Queries Windows Storage API for physical hardware status matching this drive's SN
            str raw = _powershell("Select-Object -Property HealthStatus, OperationalStatus | ConvertTo-Json");
            
            if (!raw.empty()) {
                try {
                    json data = json::parse(raw);
                    str health = data.contains("HealthStatus") ? data["HealthStatus"].get<str>() : "Unknown";
                    str opStatus = data.contains("OperationalStatus") ? data["OperationalStatus"].dump() : "";

                    // A BIOS traditionally alerts on Predictive Failure or non-Healthy states
                    if (health == "Healthy" && opStatus.find("Predictive Failure") == std::string::npos) {
                        msg << "[ OK ]\n";
                        msg << "STATUS: OK. NO BAD SECTORS OR HARDWARE FAULTS DETECTED.";
                    } else {
                        msg << "[ BAD ]\n";
                        msg << "WARNING: S.M.A.R.T. HARDWARE FAILURE PREDICTED ON DETECTED DRIVE.\n";
                        msg << "STATUS : " << health << " (" << opStatus << ")\n";
                        msg << "CRITICAL: BAD SECTORS OR HARDWARE INSTABILITY DETECTED. BACK UP DATA IMMEDIATELY!";
                    }
                    return msg.str();
                } catch (...) {
                    // Fall through to raw check if JSON parsing fails
                }
            }

            // Quick fallback method using plain text output if JSON parsing fails
            str plainStatus = _powershell("(Get-PhysicalDisk).HealthStatus");
            if (plainStatus.find("Healthy") != std::string::npos) {
                msg << "[ OK ]\nSTATUS: OK. ALL SECTORS FUNCTIONAL.";
            } else {
                msg << "[ FAILED ]\nCRITICAL: DEVICE HARDWARE STATUS IS UNHEALTHY. REPLACE DRIVE.";
            }

        #else
            // Linux / macOS standard smartctl fallback
            auto [exit_code, out_buf, err_buf] = subprocess::capture_run("smartctl", {
                "-H", 
                ("/dev/sd" + std::string(1, 'a' + hwDisk()->id()))
            });

            str output = out_buf.to_string();
            
            if (exit_code == 0 && output.find("PASSED") != std::string::npos) {
                msg << "[ OK ]\nSTATUS: OK. DRIVE IS HEALTHY.";
            } else {
                msg << "[ FAILED ]\n";
                msg << "WARNING: S.M.A.R.T. HARDWARE HEALTH CHECK FAILED.\n";
                msg << "CRITICAL: SURFACE SECTOR FAILURE OR HARDWARE DEGRADATION DETECTED.";
            }
        #endif

        return msg.str();
    }

    //===============================================================================

};
