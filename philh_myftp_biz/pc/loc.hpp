#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <filesystem>

namespace fs = std::filesystem;
namespace py = pybind11;

using str = std::string;

struct PathTypeHint {
    py::object i;
};

namespace pybind11 { namespace detail {
    template <> struct type_caster<PathTypeHint> {
    public:
        PYBIND11_TYPE_CASTER(PathTypeHint, const_name("Path"));

        static handle cast(PathTypeHint src, return_value_policy /* policy */, handle /* parent */) {
            return src.i.release();
        }
    };
}}

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
    
    PathTypeHint get_temp() {

        py::object gettempdir = py::module_::import("tempfile").attr("gettempdir");
        
        py::object SERVER = _Path("E:/__temp__/");

        if (SERVER.attr("exists").cast<bool>()) {
            return PathTypeHint{ SERVER };
        } else {
            return PathTypeHint{ _Path(gettempdir()) };
        }

    }

    PathTypeHint get_script() {

        py::object mod = get_modules()["__main__"];

        if (py::hasattr(mod, "__file__")) {
            py::object parent_path = _Path(mod.attr("__file__")).attr("parent");
            return PathTypeHint{ parent_path };
        } else {
            py::object current_path = _Path(fs::current_path().string());
            return PathTypeHint{ current_path };
        }

    }

    PathTypeHint get_cache() {

        py::object path = get_script().i.attr("child")(py::str("/__pycache__/"));
        
        path.attr("mkdir")();

        return PathTypeHint{ path };
    }

};
