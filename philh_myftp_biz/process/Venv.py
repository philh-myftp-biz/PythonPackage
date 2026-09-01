from .SubProcess import TerminalMap, _TerminalMap
from functools import cached_property
from ..pc.Path import Path

_exepaths = [
    "/Scripts/python.exe",
    "/python.exe"
]

class SubVenv(Path):
    """Set Venv for SubProcess"""

    @cached_property
    def exe(self) -> Path:
        return next(filter(
            lambda p: p.exists,
            (self.child(p) for p in _exepaths)
        ))

    def enable(self):
        TerminalMap['py']['args'] = [self.exe.path]
        TerminalMap['pym']['args'] = [self.exe.path, '-m']

    def disable(self): 
        TerminalMap['py']  = _TerminalMap['py']
        TerminalMap['pym'] = _TerminalMap['pym']
