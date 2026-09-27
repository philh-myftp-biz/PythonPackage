from pybind_setup_ext import cpp_ext, setup, update_submodule
from pybind_setup_ext.cpp_ext import _kw_templ

update_submodule('headers', force=True, remote=True)

_kw_templ['include_dirs'] += ['headers']

setup(

    cpp_ext("philh_myftp_biz/num.cpp"),
    cpp_ext("philh_myftp_biz/web/_web.cpp"),
    cpp_ext("philh_myftp_biz/time/_time.cpp"),
    cpp_ext("philh_myftp_biz/text/uio.cpp"),
    cpp_ext("philh_myftp_biz/text/hex.cpp"),
    cpp_ext("philh_myftp_biz/text/contains.cpp"),
    cpp_ext("philh_myftp_biz/pc/_pc.cpp"),

    cpp_ext(
        "philh_myftp_biz/pc/hardware.cpp",
        platforms = ['win32'],
    ),

    cpp_ext(
        "philh_myftp_biz/pc/hardware.cpp",
        platforms = ['linux', 'darwin'],
        extra_objects = [
            "headers/hwinfo/*.a",
            "headers/pciutils/*.a",
        ],
        extra_link_args = ["-lz", "-lresolv"],
    ),

)

