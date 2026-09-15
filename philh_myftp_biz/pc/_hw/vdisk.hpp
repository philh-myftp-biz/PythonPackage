#include <string>
#include <iostream>
#include <filesystem>

#include <json.hpp>

namespace fs = std::filesystem;

struct VirtualDisk {

    std::string Name;
    std::string Mount;

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

    bool Connected() {
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
