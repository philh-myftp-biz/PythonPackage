from setuptools.command.build_ext import build_ext
from pybind11 import get_include
from os.path import abspath
from sys import platform
from pathlib import Path
from mypy import stubgen
import setuptools as st
import sys

class BuildExtWithStubs(build_ext):
    def run(self) -> None:
        
        super().run()

        sys.path.insert(0, abspath(self.build_lib))

        for ext in self.extensions:
            stubgen.generate_stubs(stubgen.parse_options([
                '-m', ext.name,
                '-o', abspath(self.build_lib),
                '--inspect-mode'
            ]))

ext_modules = []

ext_kw = {
    'extra_compile_args': (["/std:c++20", "/EHsc"] if platform=="win32" else ["-std=c++20"]),
    'language': 'c++',
}

for cpp in Path("philh_myftp_biz").rglob("*.cpp"):

    ext_modules += [st.Extension(
        name = cpp.as_posix().rsplit('.', 1)[0].replace('/', '.'),
        sources = [cpp.as_posix()],
        include_dirs = [
            cpp.parent.as_posix(),
            get_include()
        ],
        **ext_kw
    )]

st.setup(
    ext_modules = ext_modules,
    cmdclass = {'build_ext': BuildExtWithStubs}
)

