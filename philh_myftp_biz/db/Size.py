from functools import cache
from typing import Literal
from sys import maxsize

units = Literal[
    'B',
    'KB',
    'MB',
    'GB',
    'TB'
]
"""Type hint for keys in size.conv_factors"""

conv_factors: dict[str, int] = {
    'B' : 1,
    'KB': 1024,
    'MB': 1024**2,
    'GB': 1024**3,
    'TB': 1024**4
}
"""
Conversion Factors for file sizes

EXAMPLE:
size.conv_factors['B'] -> 1
size.conv_factors['KB'] -> 1024
"""

@staticmethod
@cache
def to_bytes(string:str) -> float:
    """
    Convert Size String to bytes

    EXAMPLE:
    size.to_bytes('10B') -> 10
    size.to_bytes('10GB')
    """
    from re import search

    match = search(
        pattern = r"(\d+(\.\d+)?)\s*([a-zA-Z]+)",
        string = string.strip()
    )

    value = float(match.group(1))

    unit = match.group(3).upper()
    unit = unit[0] + unit[-1]

    return (value * conv_factors[unit])

def from_bytes(
    value: float,
    unit: units | None = None,
    ndigits: int = maxsize
) -> str:
    """
    Get Size String from bytes

    If unit is not given, then the unit will be automatically determined
    """

    format = lambda unit: round(
        number = (float(value) / conv_factors[unit]),
        ndigits = ndigits
    )

    if unit is None:
                    
        for unit in reversed(conv_factors):
            
            if format(unit) >= 1:            
                
                break
            
    return f'{format(unit=unit)} {unit}'
