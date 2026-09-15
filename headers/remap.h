#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
    #define WIN32_LEAN_AND_MEAN
#else
    #define LINUX
#endif

#if __has_include(<pybind11/pybind11.h>)
    #include <pybind11/pybind11.h>
    namespace py = pybind11;
    using pyobj = pybind11::object;
    using pymod = pybind11::module_;
    inline auto& import = pybind11::module_::import;
#endif

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#if __has_include(<json.hpp>)
    #include <json.hpp>
    using json = nlohmann::json;
#endif

#if __has_include(<optional>)
    #include <optional>
    template <typename T> using optional = std::optional<T>;
    inline auto& nullopt = std::nullopt;
#endif

#if __has_include(<string>)
    #include <string>
    using str = std::string;
    using wstr = std::wstring;
#endif

#if __has_include(<hwinfo/hwinfo.h>)
    #include <hwinfo/hwinfo.h>
    #ifdef WINDOWS
        namespace hwWMI = hwinfo::utils::WMI;
    #endif
#endif

#if __has_include(<chrono>)
    #include <chrono>
    namespace chrono = std::chrono;
    using namespace std::chrono_literals;
#endif

#if __has_include(<thread>)
    #include <thread>
    namespace thread = std::this_thread;
#endif

