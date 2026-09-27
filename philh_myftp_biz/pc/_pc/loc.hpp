#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <filesystem>
#include <string>

#include "remap.h"

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

class _loc { public:

    py::object _Path(const auto path) {
        py::object mod = import("philh_myftp_biz.pc.Path");
        py::object cls = mod.attr("Path");
        return cls(py::str(path));
    }
    
    PathTypeHint get_temp() {

        py::object gettempdir = import("tempfile").attr("gettempdir");
        
        py::object SERVER = _Path("E:/__temp__/");

        if (SERVER.attr("exists").cast<bool>()) {
            return PathTypeHint{ SERVER };
        } else {
            return PathTypeHint{ _Path(gettempdir()) };
        }

    }

    PathTypeHint get_script() {

        py::object mod = import("sys").attr("modules")["__main__"];

        fs::path pure;

        if (py::hasattr(mod, "__file__")) {
            str _file = mod.attr("__file__").cast<str>();
            pure = fs::path(_file).parent_path();
        } else {
            pure = fs::current_path();
        }

        pyobj wrapped = _Path(pure.string());
        return PathTypeHint{ wrapped };
    }

    PathTypeHint get_cache() {

        py::object path = get_script().i.attr("child")(py::str("/__pycache__/"));
        
        path.attr("mkdir")();

        return PathTypeHint{ path };
    }

};
