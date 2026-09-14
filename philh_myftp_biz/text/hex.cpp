#include <pybind11/pybind11.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cctype>

#include "remap.h"

// Convert any pickleable Python object into a hex string
std::string encode(const py::object& value) {
    // Call dill.dumps(value) to get bytes object
    py::bytes py_bytes = import("dill").attr("dumps")(value);
    
    // Convert Python bytes to C++ std::string
    std::string binary_str = py_bytes;
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char c : binary_str) {
        oss << std::setw(2) << static_cast<int>(c);
    }
    return oss.str();
}

// Convert hexadecimal string back into the original Python value
py::object decode(const std::string& value) {
    if (value.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even length");
    }

    // Convert hex string to raw binary string data
    std::string bytes;
    bytes.reserve(value.length() / 2);
    
    try {
        for (size_t i = 0; i < value.length(); i += 2) {
            std::string byte_string = value.substr(i, 2);
            // FIX: std::stoi correctly reads the std::string slice
            char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
            bytes.push_back(byte);
        }
    } catch (const std::exception&) {
        throw std::invalid_argument("String contains invalid hexadecimal characters");
    }
    
    // Reconstruct Python bytes object and pass to dill.loads()
    py::bytes py_bytes(bytes);
    return import("dill").attr("loads")(py_bytes);
}

// Check if string is a structurally valid hexadecimal dump
bool valid(const std::string& string) {
    // FIX: Check length and characters safely. 
    // Never call dill.loads on untrusted data just to check validation!
    if (string.empty() || string.length() % 2 != 0) {
        return false;
    }
    
    for (char c : string) {
        if (!std::isxdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

PYBIND11_MODULE(hex, m) {

    m.def("encode", &encode, 
        "Convert any pickleable object into a string",
        py::arg("value")
    );

    m.def("decode", &decode, 
        "Convert hexadecimal string back into original value",
        py::arg("value")
    );

    m.def("valid", &valid, 
        "Check if string is a valid hexadecimal dump layout",
        py::arg("string")
    );

    m.attr("PickleErrors") = py::make_tuple(
        py::handle(PyExc_ValueError),
        py::handle(PyExc_TypeError),
        py::handle(PyExc_AttributeError),
        py::handle(PyExc_EOFError),
        py::handle(PyExc_IndexError),
        py::handle(PyExc_KeyError),
        import("pickle").attr("UnpicklingError")
    );

}
