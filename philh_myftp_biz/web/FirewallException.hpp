#include <pybind11/pybind11.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "remap.h"

class FirewallException { public:

    std::string name;

    FirewallException(const std::string& name) {
        this->name = name;
    }

    static std::string _run(const std::string& args) {
        #ifdef WINDOWS
            std::array<char, 256> buffer;
            std::string output;

            // Use _popen and _pclose on Windows to capture process output
            std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(args.c_str(), "r"), _pclose);
            if (!pipe) {
                throw std::runtime_error("_popen() failed!");
            }
            
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                output += buffer.data();
            }

            return output;
        #endif    
        return "";
    }

    std::string repr() const {
        return "FirewallException(" + name + ")";
    }

    bool exists() {
        std::string outp = _run("netsh advfirewall firewall show rule name=\"" + name + "\"");
        return outp.find("No rules match the specified criteria.") == std::string::npos;
    }

    void delete_rule() {
        _run("netsh advfirewall firewall delete rule name=\"" + name + "\"");
    }

    void set(int port, const std::string& dir = "in") {
        
        if (exists()) delete_rule();

        _run("netsh advfirewall firewall add rule name=\"" + name + 
            "\" dir=" + dir + " action=allow protocol=TCP" +
            " localport=" + std::to_string(port)
        );
    
    }

};

