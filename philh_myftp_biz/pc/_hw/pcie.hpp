#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <hwinfo/hwinfo.h>
#include <remap.h>

#include <_hw/device.h>

#include "pciutils/pciutils.hpp"

struct PCIeCard : public Device {

    int Slot; // 0, 1, 2, 3, 4, ...
    int Lanes; // 1, 4, 16
    str Name;

    pcieutils::CardDetails card;

    str GetName() const override { return this->Name; }

    //===============================================================================

    PCIeCard(
        int Slot,
        int Lanes
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;

        this->Name = "Slot " + std::to_string(Slot) + " [x" + stru::zfill(2, Lanes) + "]";

        this->card = pcieutils::get_card(Slot);
    }

    //===============================================================================
    // Connected
    
    bool GetConnected() const override {
        return !card.topology_address.empty();
    }
    
    //===============================================================================
    // HealthReport

    str GetHealthReport() const override {
        
        if (!GetConnected()) {
            return "PCIe EXPANSION SLOT STATUS... [ EMPTY / NOT RESPONDING ]\n"
                   "CRITICAL: DEVICE IN " + Name + " IS UNRESPONSIVE OR UNPOWERED.";
        }

        std::ostringstream msg;
        msg << "PCIe BUS DECODER DETECTED ADAPTER:\n";
        msg << "BUS ADDRESS        : " << card.topology_address << "\n";
        msg << "DEVICE DESCRIPTION : " << (card.hardware_name.empty() ? "Generic Adapter" : card.hardware_name) << "\n";
        msg << "VENDOR / PRODUCT ID: " << std::hex << std::setw(4) << std::setfill('0') << card.vendor_id 
            << ":" << std::setw(4) << std::setfill('0') << card.device_id << std::dec << "\n";
        msg << "LINK NEGOTIATION CHECKING... ";

        // Validates physical links mapped onto the system architecture
        if (card.class_code != 0) {
            msg << "[ OK ]\n";
            msg << "STATUS : OK. LINK OPERATING AT FULL SLOT WIDTH CAPACITY (x" << Lanes << ").\n";
            msg << "ERRORS : 0 SYSTEM BUS WHEA / AER FAULTS REPORTED.";
        } else {
            msg << "[ DEGRADED ]\n";
            msg << "WARNING: BUS ENUMERATION DROPPED ATTRIBUTES.\n";
            msg << "CRITICAL: CLEARED PACKET DROP THRESHOLDS BREACHED.";
        }

        return msg.str();
    }
    //===============================================================================

};
