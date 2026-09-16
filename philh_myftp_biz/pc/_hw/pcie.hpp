#pragma once

#include <string>
#include <variant>
#include <optional>
#include <iostream>
#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>  
#include <initguid.h>  

#pragma comment (lib, "setupapi.lib")
#pragma comment (lib, "cfgmgr32.lib")

#include <_hw/device.h>

struct PCIeCard : public Device {

    std::string Slot; // '1', '2', '3', '4', 'M.2'
    int Lanes; // 1, 4, 16
    std::string DeviceId;
    std::string Name;

    std::string GetName() const override { return this->Name; }

    PCIeCard(
        std::string Slot, 
        int Lanes, 
        std::string DeviceId
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;
        this->DeviceId = DeviceId;

        this->Name = Slot + " [x" + std::to_string(Lanes) + "]";
    }

    bool GetConnected() const override {
        DEVINST devInst;
        
        // FIX: Removed the problematic DEVNUM_FROM_TYPE macro cast entirely.
        // We cast the string directly to a modifiable Windows character pointer (DEVINSTID_A)
        // which matches the native signature perfectly.
        CONFIGRET status = CM_Locate_DevNodeA(
            &devInst, 
            const_cast<char*>(DeviceId.c_str()), 
            CM_LOCATE_DEVNODE_NORMAL
        );

        // CR_SUCCESS means the device tree successfully found your hardware identifier.
        return (status == CR_SUCCESS);
    }

};
