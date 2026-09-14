#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include "httplib.h"
#include "NetIF.hpp"

namespace py = pybind11;

class URL { public:

    std::string base_url;
    std::string full_path;
    std::string host;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    int max_tries;
    int max_age;
    int timeout;

    URL(
        std::string url, 
        std::map<std::string, std::string> params = {},
        std::map<std::string, std::string> headers = {},
        int max_tries = 1,
        int max_age = 0,
        int timeout = 30
    ) {

        this->params = params;
        this->headers = headers;
        this->max_tries = max_tries;
        this->max_age = max_age;
        this->timeout = timeout;

        size_t q_mark = url.find('?');
        if (q_mark != std::string::npos) {
            base_url = url.substr(0, q_mark);
            std::string q_str = url.substr(q_mark + 1);
            
            std::stringstream ss(q_str);
            std::string item;
            while (std::getline(ss, item, '&')) {
                size_t eq = item.find('=');
                if (eq != std::string::npos) {
                    params[item.substr(0, eq)] = item.substr(eq + 1);
                } else if (!item.empty()) {
                    params[item] = "";
                }
            }

        } else {
            base_url = url;
        }

        std::string scheme_marker = "://";
        size_t scheme_pos = base_url.find(scheme_marker);
        std::string without_scheme = (scheme_pos == std::string::npos) ? base_url : base_url.substr(scheme_pos + scheme_marker.length());
        
        size_t slash_pos = without_scheme.find('/');
        if (slash_pos == std::string::npos) {
            host = without_scheme;
            full_path = "/";
        } else {
            host = without_scheme.substr(0, slash_pos);
            full_path = without_scheme.substr(slash_pos);
        }

    }

    std::string to_string() const {
        if (params.empty()) return base_url;
        
        std::string result = base_url + "?";
        bool first = true;
        for (const auto& [key, val] : params) {
            if (!first) result += "&";
            result += key + "=" + val;
            first = false;
        }
        return result;
    }

    URL copy(const py::kwargs& kwargs) {

        std::string next_url = to_string();
        auto next_params = params;
        auto next_headers = headers;
        int next_tries = max_tries;
        int next_age = max_age;
        int next_timeout = timeout;

        if (kwargs.contains("url")) next_url = kwargs["url"].cast<std::string>();
        
        if (kwargs.contains("params")) {
            // Safe manual extraction that forces conversion to string
            py::dict py_params = kwargs["params"].cast<py::dict>();
            next_params.clear();
            for (auto item : py_params) {
                std::string key = py::str(item.first);
                std::string val = py::str(item.second);
                next_params[key] = val;
            }
        }
        
        if (kwargs.contains("headers")) {
            // Safe manual extraction for headers too
            py::dict py_headers = kwargs["headers"].cast<py::dict>();
            next_headers.clear();
            for (auto item : py_headers) {
                std::string key = py::str(item.first);
                std::string val = py::str(item.second);
                next_headers[key] = val;
            }
        }
        
        if (kwargs.contains("max_tries")) next_tries = kwargs["max_tries"].cast<int>();
        if (kwargs.contains("max_age")) next_age = kwargs["max_age"].cast<int>();
        if (kwargs.contains("timeout")) next_timeout = kwargs["timeout"].cast<int>();

        return URL(next_url, next_params, next_headers, next_tries, next_age, next_timeout);
    }

    URL child(const std::string& name, const py::kwargs& kwargs) {
        
        std::string stripped_base = base_url;
        if (!stripped_base.empty() && stripped_base.back() == '/')
            stripped_base.pop_back();
        
        std::string stripped_child = name;
        if (!stripped_child.empty() && stripped_child.front() == '/')
            stripped_child = stripped_child.substr(1);
        
        std::string new_url = stripped_base + "/" + stripped_child;
        
        py::dict combined_args;
        if (kwargs) combined_args = py::dict(kwargs);
        combined_args["url"] = py::cast(new_url);

        return copy(py::kwargs(combined_args));
    }

    URL format(py::args args, py::kwargs kwargs) {
        // 1. Convert the C++ std::string into a Python string object
        py::str py_url_str(this->to_string());
        
        // 2. Call Python's native .format(*args, **kwargs)
        py::str formatted_str = py_url_str.attr("format")(*args, **kwargs);
        
        // 3. Package it into kwargs and return the copied URL object
        py::dict copy_args;      
        copy_args["url"] = formatted_str;
        
        return this->copy(py::kwargs(copy_args));
    }

    httplib::Client _client() {
        //bool is_ssl = (base_url.rfind("https://", 0) == 0);
        //std::string proto = is_ssl ? "https://" : "http://";

        httplib::Client cli("http://" + host);
        cli.set_connection_timeout(timeout);
        cli.set_read_timeout(timeout);
        return cli;
    }

    httplib::Result _get(const std::string& method = "GET") {
        
        httplib::Client cli = _client();
        cli.set_follow_location(true);

        httplib::Headers h_fields;
        for (const auto& [k, v] : headers) {
            h_fields.insert({k, v});
        }

        std::string request_path = full_path;
        if (!params.empty()) {
            request_path += "?";
            bool first = true;
            for (const auto& [k, v] : params) {
                if (!first) request_path += "&";
                request_path += k + "=" + v;
                first = false;
            }
        }

        if (method == "HEAD") {
            return cli.Head(request_path, h_fields);
        }
        return cli.Get(request_path, h_fields);
    }

    std::string text() {
        if (auto res = _get("GET")) {
            return res->body;
        }
        throw std::runtime_error("Connection Error or Timeout occurred");
    }

    py::object json() {
        py::object json_mod = py::module_::import("json");
        return json_mod.attr("loads")(text());
    }

    bool exists() {
        if (auto res = _get("HEAD")) {
            return res->status < 400;
        }
        return false;
    }

    size_t size() {
        if (auto res = _get("HEAD")) {
            if (res->has_header("Content-Length")) {
                return std::stoul(res->get_header_value("Content-Length"));
            }
        }
        return 0;
    }

    void download(const std::string& dest_path, bool force = true) {
        
        httplib::Client cli = _client();

        std::ofstream outfile(dest_path, std::ios::binary);
        if (!outfile.is_open()) throw std::runtime_error("Failed to open local file destination path.");

        httplib::Headers h_fields;
        for (const auto& [k, v] : headers) h_fields.insert({k, v});

        auto res = cli.Get(full_path, h_fields, 
            [&](const char *data, size_t data_length) {
                outfile.write(data, data_length);
                return true;
            }
        );

        outfile.close();
        if (!res) throw std::runtime_error("Download stream broken mid-operation.");

    }

};

