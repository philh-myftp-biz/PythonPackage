from typing import (
    runtime_checkable, Protocol,
    SupportsInt, SupportsFloat, SupportsBytes 
)

__all__ = [
    "SupportsInt",
    "SupportsFloat",
    "SupportsBytes",
    "SupportsStr",
    "SupportsJSON",
]

#========================================================

@runtime_checkable
class SupportsStr(Protocol):
    def __str__(self) -> str:
        ...

#========================================================

SupportsJSON = \
    str| \
    int| \
    float| \
    bool| \
    None| \
    dict[str, "SupportsJSON"]| \
    list["SupportsJSON"]

#========================================================