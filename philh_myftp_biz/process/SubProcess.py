from typing import Literal, TYPE_CHECKING, Any, TypedDict
from ._sp import _SubProcess
from sys import executable
from copy import deepcopy

if TYPE_CHECKING:
    from ..pc import Path

class Terminal(TypedDict):
    args: tuple[str, ...]
    exts: tuple[str, ...]

_TerminalMap: dict[str, Terminal] = {

    'cmd': {
        'args': ('cmd', '/c'),
        'exts': ('exe', 'bat')
    },

    'ps': {
        'args': ('Powershell', '-Command'),
        'exts': ()
    },

    'psfile': {
        'args': ('Powershell', '-File'),
        'exts': ('ps1',)
    },

    'py': {
        'args': (executable,),
        'exts': ('py',)
    },

    'pym': {
        'args': (executable, '-m'),
        'exts': ()
    },

    'vbs': {
        'args': ('wscript',),
        'exts': ('vbs',)
    }

}

TerminalMap = deepcopy(_TerminalMap)

class SubProcess(_SubProcess):

    _hide: bool
    _wait: bool

    def __init__(self,
        *args: 'str|Path',
        terminal: None|Literal['cmd', 'ps', 'psfile', 'py', 'pym', 'vbs'] = 'cmd',
        dir: 'Path|None' = None
    ) -> None:
        from ..array import stringify
        from ..pc import Path, cwd
        from logger2 import Log

        # =====================================

        if isinstance(terminal, str):
            _terminal = TerminalMap[terminal]

        elif terminal is None:
            ext = Path(args[0]).ext
            _terminal = next(
                (t for t in TerminalMap.values() if (ext in t['exts'])),
                TerminalMap['cmd']
            )

        args = [*_terminal['args'], *stringify(args)]
        
        # =====================================

        Log.VERB(f'Running Subprocess:\n{args=}\n{dir=}\nhide={self._hide}\nwait={self._wait}')

        super().__init__(
            args = args,
            cwd = str(dir or cwd()),
            hide = self._hide,
            wait = self._wait,
        )

    def output(self,
        format: Literal['json', 'hex'] = None,
        stream: Literal['out', 'err'] = 'out'
    ) -> 'str | dict | list | bool | Any':
        """Read the output from the Subprocess"""
        from ..text import hex
        from .. import json

        output: str = getattr(self, 'std'+stream)

        if format == 'json':
            return json.loads(output)
        
        elif format == 'hex':
            return hex.decode(output)
        
        else:
            return output

class Run(SubProcess):
    _hide = False
    _wait = True

class RunHidden(SubProcess):
    _hide = True
    _wait = True

class Start(SubProcess):
    _hide = False
    _wait = False

class StartHidden(SubProcess):
    _hide = True
    _wait = False