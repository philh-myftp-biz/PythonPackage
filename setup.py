from tempfile import gettempdir
from setuptools import setup
from subprocess import run
from sys import executable
from pathlib import Path

root = Path(__file__).parent.resolve()

pybind = Path(gettempdir()) / "philh_myftp_biz-pybind"

run([
    "git.exe", "clone",
    "--depth", "1",
    "https://github.com/MineFartS/pybind",
    str(pybind)
])

compile_command = [
    'Powershell.exe', 
    '-File', f'{pybind}/build.ps1'
]

for src in (root / "philh_myftp_biz").rglob("*.cpp"):

    dst = src.with_suffix(".pyd")

    print(f'Compiling: {src} -> {dst}')

    run(
        args = [
            *compile_command,
            '-Src', str(src),
            '-Dst', str(dst),
            '-Python', str(Path(executable).parent),
        ],
        check = True
    )

setup()
