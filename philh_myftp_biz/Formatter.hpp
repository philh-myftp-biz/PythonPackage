#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace py = pybind11;
namespace fs = std::filesystem;

using str = std::string;
using pyobj = pybind11::object;
using pymod = pybind11::module_;

class Formatter {
private:

    std::map<int, std::pair<str, str>> table = {
        {10, {"VERB", "90"}},   // Bright Gray 
        {20, {"INFO", "37"}},   // White text
        {25, {"MAIN", "33"}},   // Yellow text
        {30, {"WARN", "33"}},   // Yellow text
        {40, {"FAIL", "31"}},   // Red text
        {50, {"CRIT", "35"}}    // Magenta text
    };

    pyobj _sys = pymod::import("sys");
    pyobj _traceback_mod = pymod::import("traceback");
    pyobj _json = pymod::import("json");
    pyobj _builtins = pymod::import("builtins");

    str to_lower(str s) const {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
        return s;
    }

    bool contains(const str& haystack, const str& needle) const {
        return haystack.find(needle) != str::npos;
    }

    str strip(const str& target_str, char ch = '\n') const {
        if (target_str.empty()) return target_str;
        size_t first = target_str.find_first_not_of(ch);
        if (first == str::npos) return "";
        size_t last = target_str.find_last_not_of(ch);
        return target_str.substr(first, (last - first + 1));
    }

    fs::path _cached_wfile;

    fs::path _wfile() {
        if (_cached_wfile.empty()) {
            pyobj mod = _sys.attr("modules")["__main__"];

            fs::path pycache;
            if (py::hasattr(mod, "__file__")) {
                pycache = fs::absolute(mod.attr("__file__").cast<str>()).parent_path();
            } else {
                pycache = fs::current_path();
            }
            pycache /= "__pycache__";

            if (!fs::exists(pycache))
                fs::create_directories(pycache);

            _cached_wfile = (pycache / ("philh_myftp_biz.log"));
            std::ofstream clear_stream(_cached_wfile, std::ios::out | std::ios::trunc);

        }
        return _cached_wfile;
    }

    str _file() const {
        pyobj frame = _sys.attr("_getframe")(0);

        while (!frame.is_none()) {
            pyobj code = frame.attr("f_code");
            
            str path = code.attr("co_filename").cast<str>();
            std::replace(path.begin(), path.end(), '\\', '/');
            path = to_lower(path);

            if (contains(path, "lib/logging")
                || contains(path, "site-packages/fastapi")
                || contains(path, "site-packages/starlette")
                || contains(path, "site-packages/uvicorn")
                || contains(path, "threading.py")
                || contains(path, "contextlib.py")
                || contains(path, "site-packages/philh_myftp_biz")
            ) {
                frame = frame.attr("f_back");
                continue;
            }

            str name = fs::path(path).filename().string();
            int line = frame.attr("f_lineno").cast<int>();
            return name + ":" + std::to_string(line);
            
            // Fixed safety step: Ensure frame always progresses to avoid dead loops
            frame = frame.attr("f_back"); 
        }

        return "unknown:0";
    }

    str _message(const pyobj& record) const {
        pyobj msg = record.attr("msg");

        if (py::isinstance<py::str>(msg) 
            || py::isinstance<py::int_>(msg) 
            || py::isinstance<py::float_>(msg)
            || py::isinstance<py::bool_>(msg)
        ) return strip(py::str(msg).cast<str>(), '\n');
        
        if (py::isinstance<py::tuple>(msg)
            || py::isinstance<py::list>(msg)
            || py::isinstance<py::dict>(msg)
        ) {
            pyobj _dumps = _json.attr("dumps");
            py::arg _indent = py::arg("indent");
            return strip(_dumps(msg, _indent=2).cast<str>(), '\n');
        }
        
        pyobj _str = _builtins.attr("str");
        return strip(_str(msg).cast<str>(), '\n');
    }

    str _traceback(const pyobj& record) const {
        if (record.attr("exc_info").is_none())
            return "";
        
        // Fixed: Extract the single exception instance to comply with Python 3.10+ rules
        py::tuple exc_info = record.attr("exc_info").cast<py::tuple>();
        pyobj exception_instance = exc_info[1];

        py::list lines = _traceback_mod.attr("format_exception")(exception_instance);
        str full_trace = "";
        
        for (auto line : lines) {
            full_trace += line.cast<str>();
        }

        return strip(full_trace, '\n') + "\n";
    }

public:

    str format(const pyobj& record) {

        str name = record.attr("name").cast<str>();
        if (name != "root")
            return "";

        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        
        std::tm tm_now;
        #if defined(_WIN32) || defined(_WIN64)
            localtime_s(&tm_now, &time_t_now);
        #else
            localtime_r(&time_t_now, &tm_now);
        #endif

        std::ostringstream time_stream;
        time_stream << std::put_time(&tm_now, "%y/%m/%d %H:%M:%S") 
                    << "." << std::setw(2) << std::setfill('0') << (millis / 10);
        
        str TIME = time_stream.str();

        str FILE_TXT = _file();
        int levelno = record.attr("levelno").cast<int>();

        auto table_it = table.find(levelno);
        str LEVEL = (table_it != table.end()) ? table_it->second.first : "INFO";
        str COLOR_CODE = (table_it != table.end()) ? table_it->second.second : "37";

        str COLOR = "\033[" + COLOR_CODE + "m";
        str MESS = _message(record);
        str TRACE = _traceback(record);

        fs::path log_path = _wfile();
        std::ofstream log_file(log_path, std::ios::out | std::ios::app);
        if (log_file.is_open()) {
            log_file << "\n" << TIME << " " << FILE_TXT << " " << LEVEL << "\n" << MESS << "\n" << TRACE;
            log_file.close();
        }

        py::object _pkg = _sys.attr("modules")["philh_myftp_biz"];
        bool is_verbose = _builtins.attr("bool")(_pkg.attr("VERBOSE")).cast<bool>();

        if (is_verbose || (levelno > 10)) {
            return "\n" + COLOR + "\033[1m" + TIME + " " + FILE_TXT + " " + LEVEL + "\033[22m\n" + MESS + "\033[0m\n" + TRACE;
        }

        return "";
    }

};
