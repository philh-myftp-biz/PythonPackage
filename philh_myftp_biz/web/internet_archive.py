from ..functools import cached_property
from dataclasses import dataclass
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from ..pc import Path

class IArchiveFile:
    """Mirrors Syntax for TorrentFile"""

    def __init__(self, 
        archive: 'IArchive',
        name: str,
        **_
    ):
        self.archive = archive
        self.name = name

@dataclass
class IArchive:
    """Mirrors Syntax for Torrent"""

    id: str

    @cached_property
    def _tmpdir(self) -> 'Path':
        from ..pc import loc
        return loc.temp.child(self.id)

    @cached_property
    def raw(self):
        from internetarchive import get_item
        return get_item(self.id)

    def start(self) -> None:
        from internetarchive import download

        download(
            self.id,
            files = (f.name for f in self.files), 
            verbose = True, 
            destdir = self._tmpdir
        )

    @cached_property
    def files(self) -> tuple[IArchiveFile, ...]:
        return tuple(IArchiveFile(self, **f) for f in self.raw.files if f['source'] == 'original')

