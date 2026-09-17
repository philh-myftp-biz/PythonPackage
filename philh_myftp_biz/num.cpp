#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include "remap.h"

// ========================================================
// Helper Function: nlen
// If num is float|int, returns num. If num is Iterable, returns len(num).
// ========================================================
int nlen(py::object num) {
    if (py::isinstance<py::int_>(num) || py::isinstance<py::float_>(num))
        return num.cast<int>();
    // Fallback: assume it's an iterable and return its length
    return static_cast<int>(py::len(num));
}

// ========================================================
// Helper Function: get_digit (RENAMED to avoid conflict)
// Get digit from number by index. e.g., get_digit(123, 0) -> 1
// ========================================================
int get_digit(long long num, int i) {
    std::string s = std::to_string(num);
    
    // Handle Python-style negative indexing safely
    if (i < 0) {
        i = static_cast<int>(s.length()) + i;
    }
    
    if (i < 0 || i >= static_cast<int>(s.length())) {
        throw std::out_of_range("Index out of bounds for the given number digits.");
    }
    
    // Convert the single character digit back to an integer
    return s[i] - '0';
}

// ========================================================
// Helper Function: clamp
// Clamp a number to a range
// ========================================================
double clamp(double x, double min_val, double max_val) {
    return std::max(std::min(x, max_val), min_val);
}

// ========================================================
// Helper Function: nearest_multiple
// Snap x to the nearest multiple of a number
// ========================================================
long long nearest_multiple(double x, long long multiple_of) {
    if (multiple_of == 0) {
        throw std::invalid_argument("multiple_of cannot be zero.");
    }
    return static_cast<long long>(std::floor(x / multiple_of)) * multiple_of;
}

// ========================================================
// Type Validation Functions
// ========================================================

bool _cap_wrap(pyobj num, str cls) {
    pyobj _cls = import("builtins").attr(cls.c_str());
    try {
        _cls(num);
        return true;
    } catch (const py::error_already_set&) {
        return false;
    }
}

bool is_int(pyobj num) {
    return _cap_wrap(num, "int");
}

bool is_float(pyobj num) {
    return _cap_wrap(num, "float");
}

bool is_num(pyobj num) {
    return is_int(num) || is_float(num);
}

// ========================================================
// Helper Function: is_prime
// Check if a number is a prime number
// ========================================================
bool is_prime(py::object num_obj) {
    // Standardize to an integer for tracking math limits
    long long num;
    try {
        num = num_obj.cast<long long>();
    } catch (...) {
        return false;
    }

    if (num == 0 || num == 1) return false;
    if (num == 2) return true;
    if (num < 0) return false;

    // Check last digit rule using the renamed get_digit function
    int last_digit = get_digit(num, -1);
    if (last_digit == 0 || last_digit == 2 || last_digit == 4 || 
        last_digit == 5 || last_digit == 6 || last_digit == 8) {
        return false;
    }

    // Trial division
    for (long long i = 2; i < num; ++i) {
        if ((num % i) == 0) {
            return false;
        }
    }

    return true;
}

// ========================================================
// Pybind11 Module Definitions
// ========================================================
PYBIND11_MODULE(num, m) {

    m.def("nlen", &nlen, 
        "Returns num if it is a number, otherwise returns its length.",
        py::arg("num")
    );
          
    m.def("digit", &get_digit,
        "Get digit from number by index.",
        py::arg("num"),
        py::arg("i")
    );
          
    m.def("clamp", &clamp,
        "Clamp a number to a range.",
        py::arg("x"),
        py::arg("MIN"),
        py::arg("MAX")
    );
          
    m.def("nearest_multiple", &nearest_multiple,
        "Snap x to the nearest multiple of a number.",
        py::arg("x"),
        py::arg("multiple_of")
    );
          
    m.def("is_int", &is_int,
        "Check if number is a valid integer.",
        py::arg("num")
    );
          
    m.def("is_float", &is_float,
        "Check if a number is a valid float.",
        py::arg("num")
    );
          
    m.def("is_num", &is_num,
        "Check if number is a valid integer or float.",
        py::arg("num")
    );
          
    m.def("is_prime", &is_prime, 
        "Check if a number is a prime number.",
        py::arg("num")
    );

}
