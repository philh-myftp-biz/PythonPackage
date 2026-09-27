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

    //===============================================================================

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

    //===============================================================================
    // Connected

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

    //===============================================================================
    // Repair

    void Repair() {
        // Repair-VirtualDisk
        powershell("Repair");
    }

    //===============================================================================
    // HealthReport

    str GetHealthReport() const override {
        
        if (!GetConnected()) {
            return "VIRTUAL STORAGE CONTROLLER... [ DISCONNECTED ]\n"
                   "CRITICAL: MOUNT POINT '" + Mount + "' IS UNREACHABLE OR NOT ATTACHED.";
        }

        std::ostringstream msg;
        msg << "VIRTUAL DISK SUBSYSTEM: " << Name << "\n";
        msg << "TARGET MOUNT VOLUME   : " << Mount << "\n";
        msg << "ARRAY REDUNDANCY VALIDATING... ";

        #ifdef WINDOWS
            // Run a PowerShell capture using your pattern to fetch HealthStatus and OperationalStatus
            str script = "Get-VirtualDisk -FriendlyName '" + Name + "' | Select-Object -Property HealthStatus, OperationalStatus | ConvertTo-Json";
            auto [exit_code, out_buf, err_buf] = subprocess::capture_run(narg::$powershell, script);

            if (exit_code == 0 && !out_buf.to_string().empty()) {
                try {
                    json data = json::parse(out_buf.to_string());
                    str health = data.contains("HealthStatus") ? data["HealthStatus"].get<str>() : "Unknown";
                    str opStatus = data.contains("OperationalStatus") ? data["OperationalStatus"].dump() : "";

                    if (health == "Healthy") {
                        msg << "[ OK ]\n";
                        msg << "STATUS: OK. ALL VIRTUAL SECTORS VALIDATED AND SYNCHRONIZED.";
                    } else if (health == "Warning" || opStatus.find("Degraded") != std::string::npos) {
                        msg << "[ DEGRADED ]\n";
                        msg << "WARNING: VIRTUAL ARRAY IS DEGRADED / INCOMPLETE.\n";
                        msg << "STATUS : " << health << " (" << opStatus << ")\n";
                        msg << "ACTION : EXECUTE SYSTEM Repair() LOOP TO INITIATE STORAGE PARITY REBUILD.";
                    } else {
                        msg << "[ FAILED ]\n";
                        msg << "CRITICAL: VIRTUAL VOLUME IS CORRUPTED OR UNDERLYING HARDWARE DIED.\n";
                        msg << "STATUS : " << health << "\n";
                        msg << "CRITICAL: CHKDSK / STORAGE REPAIR INTERVENTION MANDATORY.";
                    }
                    return msg.str();
                } catch (...) {
                    // Fall through to basic plain check if JSON parsing fails
                }
            }
            
            msg << "[ UNKNOWN ]\nWARNING: MANAGEMENT INSTRUMENTATION (WMI) ERROR SECURING STATUS.";
        #else
            // Fallback layout for non-Windows platforms
            if (fs::exists(Mount)) {
                msg << "[ OK ]\nSTATUS: MOUNT ACCESS VERIFIED. VIRTUAL BACKING FILESYSTEM RESPONDING.";
            } else {
                msg << "[ FAILED ]\nCRITICAL: FILE ARCHIVE INTEGRITY DEGRADED.";
            }
        #endif

        return msg.str();
    }


    //===============================================================================

};
