from typing import SupportsInt, SupportsFloat, Iterable

#========================================================

from sys import maxsize as maxint # pyright: ignore[reportUnusedImport]

from math import trunc, floor # pyright: ignore[reportUnusedImport]

from random import randint, randrange # pyright: ignore[reportUnusedImport]

from ._log import MutInt # pyright: ignore[reportUnusedImport]
    
#========================================================

def nlen(num:int|float|Iterable) -> int|float:
    """
    If num is float|int, returns num
    If num is Iterable, returns len(num)
    """
    if isinstance(num, (int, float)):
        return num
    else:
        return len(num)

def digit(num:int, i:int) -> int:
    """
    Get digit from number by index

    digit(123, 0) -> 1
    """

    return int( str(num) [i] )

# @dead-code-ignore
def clamp(
    x:   int|float,
    MIN: int|float,
    MAX: int|float
):
    """
    Clamp a number to a range
    """
    return max(min(x, MAX), MIN)

# @dead-code-ignore
def nearest_multiple(
    x: int|float,
    multiple_of: int
) -> int:
    """
    Snap x to the nearest multiple of a number

    EXAMPLES:
    ```
    >>> nearest_multiple(17, 8)
    16

    >>> nearest_multiple(22, 10)
    20
    ```
    """

    return int(x//multiple_of) * multiple_of

#========================================================

def is_num(num:SupportsFloat|SupportsInt) -> bool:
    """Check if number is a valid integer or float"""

    return (is_int(num) or is_float(num))

def is_int(num:SupportsInt) -> bool:
    """Check if number is a valid integer"""
    try:
        int(num)
        return True
    except ValueError:
        return False

def is_float(num:SupportsFloat) -> bool:
    """Check if a number is a valid float"""
    try:
        float(num)
        return True
    except ValueError:
        return False

# @dead-code-ignore
def is_prime(
    num: SupportsInt|SupportsFloat
) -> bool:
    """Check if a number is a prime number"""

    pre: dict[int, bool] = {
        0: False,
        1: False,
        2: True
    }

    if num in pre:
        return pre[num]

    else:

        if digit(num=num, i=-1) in [0, 2, 4, 5, 6, 8]:
            return False
        
        else:
            
            for i in range(2, num):
                if (num % i) == 0:
                    return False

            return True

#========================================================