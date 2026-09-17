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

#ifdef WINDOWS
    #include <WinReg/WinReg.hpp>
    #pragma comment(lib, "Advapi32.lib")
#endif

struct HardDrive : public Device {

    static std::vector<HardDrive> search() {
        std::vector<HardDrive> _hdds;

        for (const auto& _hwDisk : hwinfo::getAllDisks()) {
            
            str _sn = stru::strip( _hwDisk.serial_number() );
            if (_sn.starts_with('{')) continue;
            
            HardDrive _hdd = HardDrive(
                "?", // Tower
                "?", // COnn
                -1, // ID
                _sn // SN
            );
            _hdds.push_back(_hdd);
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

        str _powershell(str cmd) {
            str script = "Get-PhysicalDisk | Where-Object SerialNumber -eq '" + SN + "' | " + cmd;
            
            auto [exit_code, out_buf, err_buf] = subprocess::capture_run(narg::$powershell, script);
            
            if (exit_code == 0) {
                return out_buf.to_string();
            } else {
                return "";
            }
        }

        optional<wstr> PNPDeviceID() {
            std::vector<str> pnpIds = hwWMI::query<str>(L"Win32_DiskDrive", L"PNPDeviceID");
            std::vector<str> serials = hwWMI::query<str>(L"Win32_DiskDrive", L"SerialNumber");

            for (size_t i = 0; i < serials.size() && i < pnpIds.size(); ++i) {
                if (stru::match_str(serials[i], SN))
                    return stru::to_wstr(pnpIds[i]);
            }

            return nullopt;
        }

    #endif    

    //===============================================================================
    // hwDisk

    mutable optional<hwinfo::Disk> _cached_hwDisk = nullopt;

    optional<hwinfo::Disk> hwDisk() const {
        if (!_cached_hwDisk.has_value()) {
            for (const auto& _hwDisk : hwinfo::getAllDisks()) {
                if (_hwDisk.serial_number() == SN) {
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

    bool GetConnected() const {
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

    #ifdef WINDOWS
        std::unique_ptr<winreg::RegKey> _friendly_name_reg() {
            auto pnpOpt = PNPDeviceID();
            if (!pnpOpt.has_value()) return nullptr;
            return std::make_unique<winreg::RegKey>(
                HKEY_LOCAL_MACHINE, 
                (L"SYSTEM\\ControlSet001\\Enum\\" + pnpOpt.value())
            );
        }
    #endif

    str FriendlyName() {
        if (!GetConnected()) return "";
        #ifdef WINDOWS
            auto key = _friendly_name_reg();
            if (!key) return "";
            wstr wname = key->GetStringValue(L"FriendlyName");
            return stru::to_str(wname);
        #else
            return hwDisk()->model();
        #endif
    }

    void setFriendlyName(str name) {
        if (!GetConnected()) return;
        #ifdef WINDOWS
            auto key = _friendly_name_reg();
            if (!key) return;
            wstr wname = stru::to_wstr(name);
            key->SetStringValue(L"FriendlyName", wname);
        #endif
    }

    //===============================================================================
    // Usage

    str Usage() {
        if (!GetConnected()) return "";

        #ifdef WINDOWS
            str raw = _powershell("Select-Object -Property Usage | ConvertTo-Json"); 
            json data = json::parse(raw);
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

};
