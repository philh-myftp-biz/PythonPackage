#include <pybind11/pybind11.h>
#include <string>
#include <mutex>
#include <iostream>
#include <_h/httplib.h>
#include <_h/NetIF.hpp>

namespace py = pybind11;

class _IP { public:

    std::string LAN() {

        std::vector<std::string> addrs = gmlc::netif::getInterfaceAddressesV4();

        if (addrs.empty()) return "";

        for (const auto& ip : addrs) {
            if (ip == "127.0.0.1") continue;
            if (ip.rfind("169.254.", 0) == 0) continue;
            return ip;
        }

        return "";
    }

    std::string WAN() {
        
        httplib::Client cli("http://api.ipify.org");

        auto res = cli.Get("/");
        if (res && res->status == 200) {
            return res->body;
        }

        return "";
    }

};

