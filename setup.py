from pybind11 import get_include
from pathlib import Path
from sys import platform
import setuptools as st

kw = {
    'name': "philh_myftp_biz",
    'packages': st.find_namespace_packages(exclude=["build*", "dist*", "tests*", "docs*"]),
    'ext_modules': []
}

if platform == "win32":
    extra_compile_args = ["/std:c++20", "/EHsc"]
else:
    extra_compile_args = ["-std=c++20"]

for cpp in Path(kw['name']).rglob("*.cpp"):

    kw["ext_modules"] += [st.Extension(
        name = cpp.as_posix().rsplit('.', 1)[0].replace('/', '.'),
        sources = [cpp.as_posix()],
        include_dirs = [
            cpp.parent.as_posix(),
            get_include()
        ],
        extra_compile_args = extra_compile_args,
        language = 'c++'
    )]

st.setup(**kw)
