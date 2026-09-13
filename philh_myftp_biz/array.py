from typing import Callable, Union, TypeAlias, TypeVar

_T = TypeVar('_T')

SortFunc: TypeAlias = Callable[
    [_T],
    Union[
        int, 
        float, 
        list[int | float], 
        tuple[int | float]
    ]
]

FilterFunc: TypeAlias = Callable[[_T], bool]

#========================================================

def copy(
    array: list|tuple
) -> list:
    
    if isinstance(array, list):
        return array.copy()
    
    else:
        return list(array[:])

def is_sublist(sub:list, main:list):
    n = len(sub)
    return any(main[i : i + n] == sub for i in range(len(main) - n + 1))

def stringify(array:list) -> list[str]:

    array = copy(array)

    for x, item in enumerate(array):
        array[x] = str(item)

    return array

