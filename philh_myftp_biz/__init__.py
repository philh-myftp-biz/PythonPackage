from logging import basicConfig as __basicConfig
from sys import argv as __argv
import logging as nlog
from . import _log

#================================================================

def _arg(*name:str):
    return len(set(name) & set(__argv))

HELP: bool = _arg('-h', '--help')

VERBOSE = _log.VERBOSE( _arg('-v', '--verbose') )

#================================================================

class _Formatter(_log.Formatter, nlog.Formatter):
    def __init__(self) -> None:
        _log.Formatter.__init__(self)
        nlog.Formatter.__init__(self)

class _StreamHandler(nlog.StreamHandler):
    def __init__(self) -> None:
        super().__init__()
        self.terminator = ''
        self.setFormatter(_Formatter())
        self.setLevel(10)

__basicConfig(
    level = 10,
    handlers = [_StreamHandler()]
)

#================================================================