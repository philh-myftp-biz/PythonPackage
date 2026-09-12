#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <string>
#include <algorithm>
#include <filesystem>

namespace py = pybind11;
namespace fs = std::filesystem;

class _Path { public:

    std::string path;
    std::string wpath;
    fs::path _pure;

    _Path(py::object raw_path) {

        path = _parse(raw_path);

        wpath = path;
        std::replace(wpath.begin(), wpath.end(), '/', '\\');
        
        _pure = fs::path(path);

    }

    static std::string _parse(py::object path_obj) {

        std::string fpath;
        
        // Duck-type extraction to inspect custom python types
        if (py::hasattr(path_obj, "path")) {
            fpath = path_obj.attr("path").cast<std::string>();
        
        } else if (py::hasattr(path_obj, "as_posix")) {
            fpath = path_obj.attr("as_posix")().cast<std::string>();
        
        } else {
            fpath = py::str(path_obj).cast<std::string>();
        }

        // Normalize path markers
        std::replace(fpath.begin(), fpath.end(), '\\', '/');
        while (fpath.find("//") != std::string::npos) {
            fpath.replace(fpath.find("//"), 2, "/");
        }
        
        // Match absolute pathing lookup
        py::object os_path = py::module_::import("os.path");
        fpath = os_path.attr("abspath")(fpath).cast<std::string>();
        std::replace(fpath.begin(), fpath.end(), '\\', '/'); // Clean path output again

        // Handle trailing slash rules if targeting a directory
        if (os_path.attr("isdir")(fpath).cast<bool>() && (fpath.empty() || fpath.back() != '/')) {
            fpath += '/';
        }

        return fpath;
    }

    std::string name() {
        if (is_file()) {
            return _pure.stem().string();
        } else if (is_dir()) {
            return _pure.parent_path().filename().string();
        }
    }

    std::string ext() {
        std::string _ext = _pure.extension().string();
        if (!_ext.empty() && _ext.front() == '.')
            _ext.erase(0, 1);
        std::transform(_ext.begin(), _ext.end(), _ext.begin(), ::tolower);
        return _ext;
    }

    bool exists() const {
        std::error_code ec;
        return fs::exists(path, ec);
    }

    bool is_file() const {
        std::error_code ec;
        return fs::is_regular_file(path, ec);
    }

    bool is_dir() const {
        if (!path.empty() && path.back() == '/')
            return true;
        std::error_code ec;
        return fs::is_directory(path, ec);
    }

    std::string __str__() const {
        return path;
    }

};
