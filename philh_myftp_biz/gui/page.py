from typing import TYPE_CHECKING, Self
from dataclasses import dataclass

if TYPE_CHECKING:
    from .Widget import Widget
    from .window import Window

__all__ = [
    "Page",
]

@dataclass
class Page(list['Widget']):

    gui: 'Window'

    def __iadd__(self, value: 'Widget') -> Self:
        return super().__iadd__([value])
