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
    std::string name;
    std::string ext;
    fs::path _pure;

    _Path(py::object raw_path) {

        path = _parse(raw_path);

        wpath = path;
        std::replace(wpath.begin(), wpath.end(), '/', '\\');
        
        _pure = fs::path(path);
        
        name = _pure.stem().string();

        ext = _pure.extension().string();
        if (!ext.empty() && ext.front() == '.')
            ext.erase(0, 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

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

    bool exists() const {
        return fs::exists(path);
    }

    bool is_file() const {
        return fs::is_regular_file(path);
    }

    bool is_dir() const {
        return (!path.empty() && path.back() == '/') || fs::is_directory(path);
    }

    std::string __str__() const {
        return path;
    }

};

PYBIND11_MODULE(_Path, m) {

    py::class_<_Path>(m, "_Path", py::dynamic_attr())
        .def(py::init<py::object>())
        .def_static("_parse", &_Path::_parse)
        .def("__str__", &_Path::__str__)
        .def("__repr__", &_Path::__str__)
        .def_readonly("path", &_Path::path)
        .def_readonly("wpath", &_Path::wpath)
        .def_readonly("name", &_Path::name)
        .def_readonly("ext", &_Path::ext)
        .def_readonly("_pure", &_Path::_pure)
        .def_property_readonly("exists", &_Path::exists)
        .def_property_readonly("is_file", &_Path::is_file)
        .def_property_readonly("is_dir", &_Path::is_dir);

}

