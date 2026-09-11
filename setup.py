from tempfile import gettempdir
from setuptools import setup
from subprocess import run
from sys import executable
from pathlib import Path

root = Path(__file__).parent.resolve()

pybind = Path(gettempdir()) / "philh_myftp_biz-pybind"

mtime = lambda p: p.stat().st_mtime

pydfiles: list[tuple[Path, Path]] = []

for src in (root / "philh_myftp_biz").rglob("*.cpp"):

    dst = src.with_suffix(".pyd")

    if dst.exists():
        
        _cfiles: list[Path] = [src, *src.parent.rglob("*.h"), *src.parent.rglob("*.hpp")]
        
        if any(mtime(h) > mtime(dst) for h in _cfiles):
            continue

    pydfiles += [(src, dst)]

if len(pydfiles) > 0:
    run([
        "git", "clone",
        "--depth", "1",
        "https://github.com/MineFartS/pybind",
        str(pybind)
    ])

compile_command = [
    'Powershell.exe', 
    '-File', f'{pybind}/build.ps1'
]

for src, dst in pydfiles:
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
