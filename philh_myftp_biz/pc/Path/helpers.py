from typing import Generator, TYPE_CHECKING
from dataclasses import dataclass

if TYPE_CHECKING:
    from ...time import from_stamp
    from . import Path

class _cd:

    __via_with = False
    
    def __enter__(self) -> None:
        self.__via_with = True

    def __exit__(self, *_) -> None:
        if self.__via_with:
            self.back()

    def __init__(self, path:'Path') -> None:
        self._target = str(path)
        self.open()

    def open(self) -> None:
        from os import getcwd, chdir

        self._back = getcwd()

        chdir(self._target)

    __call__ = open

    def back(self) -> None:
        from os import chdir
        chdir(self._back)

@dataclass
class _mtime:

    path: 'Path'

    def set(self,
        mtime: 'int|from_stamp' = None
    ) -> None:
        from ...time import now
        from os import utime

        if mtime is None:
            mtime = now().unix
        
        utime(
            str(self.path), 
            (int(mtime), int(mtime))
        )

    @property
    def current(self):
        from ...time import from_stamp
        from os import path

        return from_stamp(
            path.getmtime( str(self.path) )
        )
    
    def __int__(self):
        return int(self.current)
    
    def __float__(self):
        return float(self.current)

    @property
    def stopwatch(self):
        from ...time import Stopwatch

        SW = Stopwatch()

        SW.start_time = self.current
        
        return SW

@dataclass
class _set_access:

    path: 'Path'

    def __paths(self) -> Generator['Path', None, None]:

        yield self.path

        if self.path.is_dir:
            yield from self.path.descendants
    
    def readonly(self) -> None:
        from logger2 import Log
        from os import chmod

        if self.path.in_use:
            Log.VERB(f'Failed to Update Access: {self.path}')
        
        elif self.path.exists:
            for path in self.__paths():
                chmod(str(path), 0o644)

    def full(self) -> None:
        from ...process import RunHidden
        from logger2 import Log
        from os import chmod

        if self.path.is_dir:
            RunHidden('icacls', self.path, '/grant', 'Everyone:F', '/t', '/c', '/q')

        elif self.path.in_use:
            Log.VERB(f'Failed to Update Access: {self.path}')

        elif self.path.exists:
            chmod(str(self.path), 0o777)

@dataclass
class _visibility:

    path: 'Path'

    def hide(self) -> None:
        from win32con import FILE_ATTRIBUTE_HIDDEN
        from win32file import GetFileAttributes
        from win32api import SetFileAttributes
        from pywintypes import error

        if self.hidden:
            return

        self.path.set_access.full()

        attrs = GetFileAttributes(str(self.path))

        try:
            SetFileAttributes(
                str(self.path),
                (attrs | FILE_ATTRIBUTE_HIDDEN)
            )
        except error as e:
            raise PermissionError(*e.args)

    def show(self) -> None:
        from win32con import FILE_ATTRIBUTE_HIDDEN
        from win32file import GetFileAttributes
        from win32api import SetFileAttributes
        from pywintypes import error

        if not self.hidden:
            return

        self.path.set_access.full()

        attrs = GetFileAttributes(str(self.path))

        try:
            SetFileAttributes(
                str(self.path),
                (attrs & ~FILE_ATTRIBUTE_HIDDEN)
            )
        
        except error as e:
            raise PermissionError(*e.args)

    @property
    def hidden(self) -> None | bool:
        from win32con import FILE_ATTRIBUTE_HIDDEN
        from win32file import GetFileAttributes

        if self.path.exists:
            attrs = GetFileAttributes(str(self.path))
            return bool(attrs & FILE_ATTRIBUTE_HIDDEN)
