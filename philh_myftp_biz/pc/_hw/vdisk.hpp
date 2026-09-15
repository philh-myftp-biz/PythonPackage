#include <string>
#include <iostream>
#include <filesystem>

#include <json.hpp>
#include <_hw/device.h>

namespace fs = std::filesystem;

struct VirtualDisk: public Device {

    std::string Name;
    std::string Mount;

    std::string GetName() const override { return this->Name; }

    VirtualDisk(
        std::string Name,
        std::string Mount
    ) {
        this->Name = Name;
        this->Mount = Mount;
    }

    void powershell(std::string cmd) {
        std::system(
            ("powershell.exe -Command \"" + cmd + "-VirtualDisk -FriendlyName '" + Name + "'\"").c_str()
        );
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
