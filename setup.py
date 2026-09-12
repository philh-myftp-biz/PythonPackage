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
        
        self.run_command("build_py")        
        super().run()

        sys.path.insert(0, abspath(self.build_lib))

        for ext in self.extensions:
            stubgen.generate_stubs(stubgen.parse_options([
                '-m', ext.name,
                '-o', abspath(self.build_lib),
                '--inspect-mode'
            ]))

kw = {
    'name': "philh_myftp_biz",
    'packages': st.find_namespace_packages(exclude=["build*", "dist*", "tests*", "docs*"]),
    'ext_modules': [],
    'cmdclass': {
        'build_ext': BuildExtWithStubs
    },
}

for cpp in Path(kw['name']).rglob("*.cpp"):

    kw["ext_modules"] += [st.Extension(
        name = cpp.as_posix().rsplit('.', 1)[0].replace('/', '.'),
        sources = [cpp.as_posix()],
        include_dirs = [
            cpp.parent.as_posix(),
            get_include()
        ],
        extra_compile_args = (["/std:c++20", "/EHsc"] if platform=="win32" else ["-std=c++20"]),
        language = 'c++'
    )]

st.setup(**kw)
