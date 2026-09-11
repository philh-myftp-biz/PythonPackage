from typing import Literal, Generator
from .Path import Path, PathPair

from ._loc import loc # pyright: ignore[reportUnusedImport]

#========================================================

cwd = lambda: Path('.')
"""Get the Current Working Directory"""

def relscan(
    src: Path,
    dst: Path
) -> Generator[PathPair, None, None]:
    """
    Relatively Scan two directories

    EXAMPLE:

    C:/ - |
    (src) |
          | - Child1

    relscan(Path('C:/'), Path('D:/')) -> [{
        'src': Path('C:/Child1')
        'dst': Path('D:/Child1')
    }]
    """
    from ..functools import SharedBuffer
    from shutil import copytree
    from ..process import Thread

    buff = SharedBuffer()
    
    # Copytree dry run
    t = Thread(

        func = copytree,

        src = str(src),
        dst = str(dst), 

        dirs_exist_ok = True,
        
        # Append paths to list instead of directly copying
        copy_function = lambda s, d, **_: buff.add(PathPair(s, d))

    )

    buff.stop_when = lambda: not t.running

    yield from buff

#========================================================
# Lazy Values

NAME: str

OS: Literal['windows', 'unix']

def __getattr__(attr:str):
    from socket import gethostname
    import os

    match attr:

        case 'OS':
            return 'windows' if (os.name == 'nt') else 'unix'
        
        case 'NAME':
            return gethostname()
        
    raise AttributeError(f"module '{__name__}' has no attribute '{attr}'")

#=================================

