from __future__ import annotations
from .Path import Path

__all__: list[str] = ['loc']

class _loc:
    def __init__(self) -> None:
        ...
    @property
    def cache(self) -> Path:
        ...
    @property
    def script(self) -> Path:
        ...
    @property
    def temp(self) -> Path:
        ...

loc: _loc
