from typing import TYPE_CHECKING, Any
from zipfile import ZipFile as _ZipFile
from functools import cached_property
from dataclasses import dataclass

if TYPE_CHECKING:
    from ..pc import Path

from .RWFile import RWFile, XML, PKL, JSON, INI, YAML, TXT, CSV, TOML

#========================================================

def temp(
    name: str = 'undefined',
    ext: str = 'ph',
    id: Any = None
) -> 'Path':
    """Get a random path in the temporary directory"""
    from ..text import random
    from ..pc._pc import loc

    if id is None:
        _id = random(50)
    else:
        _id = str(id)      

    return loc.temp.child(f'{name}-{_id}.{ext}')

#========================================================

class VHDX:
    """.VHDX File"""

    def __init__(self,
        VHD: 'Path',
        MNT: 'Path',
        timeout: int = 30,
        readonly: bool = False
    ) -> None:
        
        self.VHD = VHD
        self.MNT = MNT
        
        self.timeout: int = timeout
        
        self.readonly: bool = readonly

    def mount(self) -> None:
        from ..process import RunHidden

        RunHidden(
            'Mount-VHD',
            '-Path', self.VHD,
            '-NoDriveLetter',
            '-Passthru',
            ('-ReadOnly' if self.readonly else ''),
            '| Get-Disk | Get-Partition | Add-PartitionAccessPath',
            '-AccessPath', self.MNT,
            terminal = 'ps',
            timeout = self.timeout
        )

    def dismount(self) -> None:
        from ..process import RunHidden
        
        RunHidden(
            'Dismount-DiskImage',
            '-ImagePath', self.VHD,
            terminal = 'ps',
            timeout = self.timeout
        )

        # Delete the mounting directory
        self.MNT.delete()

#========================================================

@dataclass
class ZIP:
    """.ZIP File"""

    path: 'Path'

    @dataclass
    class Member:

        _zip: _ZipFile
        path: str

        def open(self):
            return self._zip.open(self.path)

    @cached_property
    def _zip(self) -> _ZipFile:
        return _ZipFile(str(self.path))
        
    @property
    def members(self) -> list[Member]:
        return [self.Member(self._zip, n) for n in self._zip.namelist()]

    def search(self, term:str) -> list[Member]:
        """
        Search for files in the archive

        Ex: ZIP.search('test1') -> 'test123.json'
        """

        return [m for m in self.members if (term in m.path)]

    def extractFile(self,
        member: Member, 
        path: 'Path'
    ) -> None:
        """Extract a single file from the zip archive"""
        from shutil import copyfileobj

        src = member.open()
        dst = path.open('wb')

        copyfileobj(src, dst)

        src.close()
        dst.close()

    def extractAll(self, path:'Path') -> None:
        """Extract all files from the zip archive"""

        path.mkdir()

        self._zip.extractall(str(path))

#========================================================