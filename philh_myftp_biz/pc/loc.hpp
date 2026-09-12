#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <filesystem>

namespace fs = std::filesystem;
namespace py = pybind11;

using str = std::string;

class _loc {

private:

    py::dict get_modules() {
        return py::module_::import("sys").attr("modules");
    }

    py::object _Path(const auto path) {
        py::object mod = py::module_::import("philh_myftp_biz.pc.Path");
        py::object cls = mod.attr("Path");
        return cls(py::str(path));
    }

public:
    
    py::object get_temp() {

        py::object gettempdir = py::module_::import("tempfile").attr("gettempdir");
        
        py::object SERVER = _Path("E:/__temp__/");

        if (SERVER.attr("exists").cast<bool>()) {
            return SERVER;
        } else {
            return _Path(gettempdir());
        }

    }

    py::object get_script() {

        py::object mod = get_modules()["__main__"];

        if (py::hasattr(mod, "__file__")) {
            return _Path(mod.attr("__file__")).attr("parent");
        } else {
            return _Path(fs::current_path().string());
        }

    }

    py::object get_cache() {

        py::object path = get_script().attr("child")(py::str("/__pycache__/"));
        
        path.attr("mkdir")();

        return path;
    }

};
