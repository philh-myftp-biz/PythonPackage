from logging import basicConfig as __basicConfig
from sys import argv as __argv
import logging as nlog
from . import _log

#================================================================

def _arg(*name:str):
    return len(set(name) & set(__argv))

#================================================================

from .functools import singleton
from .num import MutInt

@singleton
class VERBOSE(MutInt):

    lvalue = _arg('-v', '--verbose')

    def __init__(self) -> None:
        super().__init__(None)
        self.resume()

    def pause(self) -> None:
        self.lvalue = self.value
        self.value = 0

    def resume(self) -> None:
        self.value = self.lvalue

    def enable(self) -> None:
        self.value  = 1
        self.lvalue = 1

    def disable(self) -> None:
        self.value  = 0
        self.lvalue = 0

#================================================================

HELP: bool = _arg('-h', '--help')

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