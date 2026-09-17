#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <hwinfo/hwinfo.h>
#include <remap.h>

#include <_hw/device.h>

struct PCIeCard : public Device {

    str Slot; // '1', '2', '3', '4', 'M.2'
    int Lanes; // 1, 4, 16
    str DeviceId;
    str Name;

    str GetName() const override { return this->Name; }

    PCIeCard(
        str Slot, 
        int Lanes, 
        str DeviceId
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;
        this->DeviceId = DeviceId;

        this->Name = Slot + " [x" + std::to_string(Lanes) + "]";
    }

    bool GetConnected() const override {
        for (const auto& gpu : hwinfo::getAllGPUs()) {
            if (gpu.name().find(DeviceId) != str::npos)
                return true;
        }        
        return false;
    }
    
};
