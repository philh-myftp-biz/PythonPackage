#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace py = pybind11;

// Helper function to lowercase a string
std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return s;
}

// Check if string contains ANY of the values
bool any(
    std::string str, 
    std::vector<std::string> values, 
    bool case_sensitive = false
) {
    if (!case_sensitive) {
        str = to_lower(str);
        for (auto& v : values) {
            v = to_lower(v);
        }
    }

    for (const auto& v : values) {
        if (str.find(v) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Check if string contains ALL of the values
bool all(
    std::string str, 
    std::vector<std::string> values, 
    bool case_sensitive = false
) {
    if (!case_sensitive) {
        str = to_lower(str);
        for (auto& v : values) {
            v = to_lower(v);
        }
    }

    for (const auto& v : values) {
        if (str.find(v) == std::string::npos) {
            return false;
        }
    }
    return true;
}

PYBIND11_MODULE(contains, m) {

    m.def("any", &any, 
        "Check if string contains any of the values",
        py::arg("string"), 
        py::arg("values"), 
        py::arg("case") = false
    );

    m.def("all", &all, 
        "Check if string contains all of the values",
        py::arg("string"), 
        py::arg("values"), 
        py::arg("case") = false
    );

}
