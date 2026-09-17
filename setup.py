from setuptools.command.build_ext import build_ext
from os.path import abspath, exists
from subprocess import check_call
from pybind11 import get_include
from sys import platform
from pathlib import Path
from mypy import stubgen
import setuptools as st
import sys

class BuildExtWithStubs(build_ext):

    @staticmethod
    def _gen_stubs(m:str, o:str):
        stubgen.generate_stubs(stubgen.parse_options([
            '-m', m,
            '-o', abspath(o), 
            '--inspect-mode'
        ]))

    def run(self) -> None:
        
        super().run()

        sys.path.insert(0, abspath(self.build_lib))

        for ext in self.extensions:

            self._gen_stubs(m=ext.name, o=self.build_lib)

            self._gen_stubs(m=ext.name, o='.')

if not exists("headers/README.md"):
    check_call(['git', 'submodule', 'update', '--init'])

ext_modules = []

ext_kw = {
    'extra_compile_args': ["-fvisibility=hidden"],
    'language': 'c++',
}

if platform == "win32":
    ext_kw['extra_compile_args'] += ["/std:c++20", "/EHsc"]
else:
    ext_kw['extra_compile_args'] += ["-std=c++20"]

include_dirs = [
    get_include(),
    "headers"
]

for cpp in Path("philh_myftp_biz").rglob("*.cpp"):

    ext_modules += [st.Extension(
        name = cpp.as_posix().rsplit('.', 1)[0].replace('/', '.'),
        sources = [cpp.as_posix()],
        include_dirs = [*include_dirs, cpp.parent.as_posix()],
        **ext_kw
    )]

st.setup(
    ext_modules = ext_modules,
    cmdclass = {'build_ext': BuildExtWithStubs}
)

