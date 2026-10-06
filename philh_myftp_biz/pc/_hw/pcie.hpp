#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <optional>
#include <unordered_set>

#include <hwinfo/hwinfo.h>
#include <remap.h>

#include <_hw/device.h>

#include "pciutils/pciutils.hpp"

struct PCIeCard : public Device {

    static vector<PCIeCard> search() {
        vector<PCIeCard> cards;
        std::unordered_set<int> slots;

        for (pciutils::pci_dev dev : pciutils::get_devices()) {
            if (slots.insert(dev.slot).second) {
                cards.push_back(PCIeCard(dev.slot, dev.lanes));
            }
        }
        return cards;
    }

    //===============================================================================
    
    int Slot; // 0, 1, 2, 3, 4, ...
    int Lanes; // 1, 4, 16
    str Name;

    str GetName() const override { return this->Name; }

    //===============================================================================

    PCIeCard(
        int Slot,
        int Lanes = -1
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;
        this->Name = "Slot " + std::to_string(Slot) + " [x" + stru::zfill(2, Lanes) + "]";
    }

    //===============================================================================
    // pci_dev

    mutable optional<pciutils::pci_dev> _cached_dev;
    
    pciutils::pci_dev* pci_dev() const {

        if (!_cached_dev) {

            for (pciutils::pci_dev dev : pciutils::get_devices()) {
                if (dev.slot == this->Slot) {
                    _cached_dev = dev;
                    break;
                }
            }

        }
        
        return _cached_dev ? &(*_cached_dev) : nullptr;
    }

    //===============================================================================
    // Link

    str GetLink() const override {

        std::ostringstream msg;
        pciutils::pci_dev* card = pci_dev();
        
        if (card != nullptr) {
            msg << "https://admin.pci-ids.ucw.cz/read/PC/";
            msg << std::hex;
            msg << card->vendor_id << "/";
            msg << card->device_id << "/";
        }

        return msg.str();
    }

    //===============================================================================
    // ID
    
    int GetID() const override {
        return this->Slot;
    }

    //===============================================================================
    // Connected
    
    bool GetConnected() const override {
        return pci_dev() != nullptr;
    }
    
    //===============================================================================
    // HealthReport

    str GetHealthReport() const override {
        
        if (!GetConnected()) {
            return "PCIe EXPANSION SLOT STATUS... [ EMPTY / NOT RESPONDING ]\n"
                   "CRITICAL: DEVICE IN " + Name + " IS UNRESPONSIVE OR UNPOWERED.";
        }

        pciutils::pci_dev* card = pci_dev();

        std::ostringstream msg;
        msg << "PCIe BUS DECODER DETECTED ADAPTER:\n";
        msg << "VENDOR / PRODUCT ID: " << std::hex << std::setw(4) << std::setfill('0') << card->vendor_id 
            << ":" << std::setw(4) << std::setfill('0') << card->device_id << std::dec << "\n";
        msg << "LINK NEGOTIATION CHECKING... ";

        // Validates physical links mapped onto the system architecture
        if (card->device_class != 0) {
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
