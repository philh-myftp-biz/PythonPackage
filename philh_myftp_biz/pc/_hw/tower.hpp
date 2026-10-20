#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <algorithm>

#include <pc/_hw/device.h>
#include <pc/_hw/hdd.hpp>

#include "remap.h"

using HDDPTR = const HardDrive*;
using HS = HealthStatus;

class Tower : public Device { public:

    //===============================================================================
    // Init

    str tower;
    
    Tower(str tower) {
        this->tower = tower;
    }

    //===============================================================================
    // _HardDrives

    vector<HDDPTR> _HardDrives;

    void setHardDrives(vector<HDDPTR> hdds) {
        _HardDrives.clear();
        for (HDDPTR hdd : hdds) {
            if (hdd->Tower == tower)
                _HardDrives.push_back(hdd);
        }
    }
    
    vector<HDDPTR>& getHardDrives() {
        return _HardDrives;
    }
    
    //===============================================================================
    // Connected

    bool GetConnected() const override {
        for (HDDPTR hdd : _HardDrives) {
            if (hdd->GetConnected()) 
                return true;
        }
        return false;
    }

    //===============================================================================
    // HealthStatus

    static inline const vector<HS> _stats_order = { HS::NOTGOOD, HS::WARNING, HS::HEALTHY };

    HS GetHealthStatus() const override {
        if (!GetConnected()) return HS::MISSING;

        for (HS stat : _stats_order) {
            for (HDDPTR hdd : _HardDrives) {
                if (hdd->GetHealthStatus() == stat)
                    return stat;
            }
        }

        return HS::UNKNOWN;
    }

    //===============================================================================
    // ID

    int GetID() const override {
        return std::hash<str>{}(tower);
    }

    //===============================================================================
    // Name

    str GetName() const override {
        return "Tower " + tower;
    }

    //===============================================================================

};
