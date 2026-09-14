#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <windows.h>
#include <winioctl.h>
#include <comdef.h>
#include <WbemIdl.h>
#include <initguid.h>
#include <cfgmgr32.h>
#include <devguid.h>
#include <devpkey.h>
#include <setupapi.h>
#include <std.h>

#include <json.hpp>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "cfgmgr32.lib")
#pragma comment(lib, "setupapi.lib")

using json = nlohmann::json;

struct HardDrive {

    //===============================================================================
    // Init

    std::string SN;
    std::string disk_path = "";
    std::string Index = "";
    bool Connected;
    HDEVINFO hDevInfo;
    DEVINST DevInst;
    std::string Tower;
    std::string Conn;
    std::string Name;
    int ID;

    HardDrive(
        std::string Tower,
        std::string Conn,
        int ID,
        std::string SN
    ) {
        this->Tower = Tower;
        this->Conn = Conn;
        this->ID = ID;
        this->SN = SN;

        //==================================
        // Get the Drive Path (disk_path, Index)

        for (UINT x = 0; x < 50; ++x) {

            std::string drivePath = "\\\\.\\PhysicalDrive" + std::to_string(x);
            
            // Open handle to the physical drive
            HANDLE hDevice = CreateFileA(
                drivePath.c_str(),
                0, // No access rights required to read attributes
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
            );

            if (hDevice == INVALID_HANDLE_VALUE) continue; // Drive index doesn't exist, move to next

            // Configure the query to fetch device properties
            STORAGE_PROPERTY_QUERY propertyQuery = {};
            propertyQuery.PropertyId = StorageDeviceProperty;
            propertyQuery.QueryType = PropertyStandardQuery;

            // Allocate a buffer to hold the output descriptor block
            BYTE outputBuffer[1024] = {};
            DWORD bytesReturned = 0;

            // Send the IOCTL command to the physical disk
            BOOL result = DeviceIoControl(
                hDevice,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &propertyQuery,
                sizeof(propertyQuery),
                outputBuffer,
                sizeof(outputBuffer),
                &bytesReturned,
                NULL
            );

            CloseHandle(hDevice);

            if (result) {

                STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(outputBuffer);
                
                // Check if a valid serial number offset exists
                if (deviceDescriptor->SerialNumberOffset != 0 && deviceDescriptor->SerialNumberOffset < bytesReturned) {
                    
                    // Extract the null-terminated ASCII serial number string
                    const char* rawSerial = reinterpret_cast<const char*>(outputBuffer + deviceDescriptor->SerialNumberOffset);

                    // Check for a match (case-insensitive or exact depending on preference)
                    if (trim_serial_number(rawSerial) == SN) {
                        Index = std::to_string(x);
                        disk_path = drivePath;
                        break;
                    }
                }

            }

        }

        //==================================
        // Get the Connected Status (Connected)

        Connected = !disk_path.empty();

        //==================================

        hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_DISKDRIVE, NULL, NULL, DIGCF_PRESENT);

        SP_DEVINFO_DATA devInfoData;
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        for (DWORD i = 0;; ++i) {
            if (!SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData)) break;

            char instanceId[512] = { 0 };
            if (!SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, instanceId, (DWORD)sizeof(instanceId), NULL))
                continue;

            if (std::string(instanceId).find(Index) == std::string::npos) 
                continue;

            DevInst = devInfoData.DevInst;
            break;

        }

        //==================================
        // Name

        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << ID << "-" << Tower;
        oss << " [" << Index << ", " << SN << "]";
        Name = oss.str();

    }

    void cleanup() {
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    //===============================================================================
    // Helper functions to trim whitespaces
    
    std::string trim_serial_number(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    std::string powershell(std::string cmd) {

        std::string command = "powershell.exe -Command \"Get-PhysicalDisk | Where-Object SerialNumber -eq '"+SN+"' | "+cmd+"\"";
        std::string result = "";
        
        // Open the pipe and run the command (Windows-specific)
        FILE* pipe = _popen(command.c_str(), "r");
        
        // Read the output line by line
        std::array<char, 128> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
        _pclose(pipe);
        return result;
    }

    //===============================================================================
    // FriendlyName

    std::string FriendlyName() {
        if (disk_path.empty()) return "";

        WCHAR wideName[512] = { 0 };
        ULONG outLen = (ULONG)sizeof(wideName);
        if (CM_Get_DevNode_Registry_PropertyW(DevInst, CM_DRP_FRIENDLYNAME, 0, (PVOID)wideName, &outLen, 0) == CR_SUCCESS)
            return std::to_string(wideName);

        return "";
    }

    void setFriendlyName(std::string name) {

        powershell("Set-PhysicalDisk -NewFriendlyName '"+name+"'");

        if (disk_path.empty()) return;

        std::wstring wideName = std::to_wstring(name);

        CM_Set_DevNode_Registry_PropertyW(
            DevInst,
            CM_DRP_FRIENDLYNAME,
            wideName.data(),
            (ULONG)(wideName.size() * sizeof(WCHAR)),
            0
        );

    }

    //===============================================================================
    // Usage

    std::string Usage() {

        std::string jsonRaw = powershell("ConvertTo-Json");

        json diskData = json::parse(jsonRaw);

        return diskData["Usage"].get<std::string>();
    }

    void setUsage(std::string usage) {
        powershell("Set-PhysicalDisk -Usage '"+usage+"'");
    }

    //===============================================================================

};
