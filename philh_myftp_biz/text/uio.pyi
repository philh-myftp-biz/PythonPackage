from __future__ import annotations
import typing
__all__: list[str] = ['UnconsumingIO']
class UnconsumingIO:
    def __init__(self, stream: typing.Any) -> None:
        ...
    def read(self, size: typing.Any = None) -> str:
        ...
    def write(self, arg0: str) -> None:
        ...
