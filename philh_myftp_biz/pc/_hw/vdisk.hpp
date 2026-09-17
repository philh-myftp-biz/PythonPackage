#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#include <json.hpp>
#include <_hw/device.h>
#include <subprocess.hpp>
#include <remap.h>

struct VirtualDisk : public Device {

    str Name;
    str Mount;

    str GetName() const override { return this->Name; }

    VirtualDisk(
        str Name,
        str Mount
    ) {
        this->Name = Name;
        this->Mount = Mount;
    }

    void powershell(str cmd) {
        #ifdef WINDOWS
            subprocess::run(
                narg::$powershell, 
                (cmd + "-VirtualDisk -FriendlyName '" + Name + "'")
            );
        #endif
    }

    bool GetConnected() const override {
        return fs::exists(Mount);
    }

    void setConnected(bool connected) {
        if (connected) {
            // Connect-VirtualDisk
            powershell("Connect");
        } else {
            // Disconnect-VirtualDisk
            powershell("Disconnect");
        }
    }

    void Repair() {
        // Repair-VirtualDisk
        powershell("Repair");
    }

};
