#include <string>
#include <variant>
#include <optional>
#include <iostream>
#include <windows.h>
#include <setupapi.h>

#pragma comment (lib, "setupapi.lib")

struct PCIeCard {

    std::string Slot; // '1', '2', '3', '4', 'M.2'
    int Lanes; // 1, 4, 16
    std::string DeviceId;
    std::string Name;

    PCIeCard(
        std::string Slot, 
        int Lanes, 
        std::string DeviceId
    ) {
        this->Slot = Slot;
        this->Lanes = Lanes;
        this->DeviceId = DeviceId;

        this->Name = Slot+" [x"+std::to_string(Lanes)+"]";
    }

    bool Connected() {
        // Keep NULL filters to search all branches, but use the 'A' (ANSI) variant
        HDEVINFO hDevInfo = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
        
        if (hDevInfo == INVALID_HANDLE_VALUE) {
            return false;
        }

        SP_DEVINFO_DATA devInfoData;
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        DWORD i = 0;
        bool isConnected = false;

        // Enumerate every physical hardware device using narrow characters
        while (SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData)) {
            char instanceId[MAX_DEVICE_ID_LEN];
            
            // Retrieve the narrow string Device Instance ID
            if (SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, instanceId, MAX_DEVICE_ID_LEN, NULL)) {
                // Case-insensitive narrow string comparison
                if (_stricmp(DeviceId.c_str(), instanceId) == 0) {
                    isConnected = true;
                    break;
                }
            }
            i++;
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);
        return isConnected;
    }

};
