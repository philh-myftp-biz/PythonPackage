from pybind_setup_ext import cpp_ext, setup, update_submodule

update_submodule('headers', force=True, remote=True)

kw = {
    'include_dirs': ["headers"]
}

setup(
    cpp_ext("philh_myftp_biz/_log.cpp", **kw),
    cpp_ext("philh_myftp_biz/num.cpp", **kw),
    cpp_ext("philh_myftp_biz/web/_web.cpp", **kw),
    cpp_ext("philh_myftp_biz/time/_time.cpp", **kw),
    cpp_ext("philh_myftp_biz/text/uio.cpp", **kw),
    cpp_ext("philh_myftp_biz/text/hex.cpp", **kw),
    cpp_ext("philh_myftp_biz/text/contains.cpp", **kw),
    cpp_ext("philh_myftp_biz/pc/hardware.cpp", **kw),
    cpp_ext("philh_myftp_biz/pc/_pc.cpp", **kw),
)

