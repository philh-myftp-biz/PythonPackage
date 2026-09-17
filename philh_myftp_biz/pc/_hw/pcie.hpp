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

    PCIeCard(
        int Slot,
        int Lanes
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;

        this->Name = "Slot " + std::to_string(Slot) + " [x" + stru::zfill(2, Lanes) + "]";

        this->card = pcieutils::get_card(Slot);
    }

    bool GetConnected() const override {
        return !card.topology_address.empty();
    }
    
};
