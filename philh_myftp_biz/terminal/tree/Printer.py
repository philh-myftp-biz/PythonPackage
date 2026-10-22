from ...functools.force_types import force_in_types
from ...db import Color

@force_in_types
def RunFile(
    path: str,
    args: tuple = ()
) -> None:
    print(f'Running: {path} {args}')

@force_in_types
def Error(
    name: str,
    mess: str = ''
) -> None:        
    print(f'<{name}> {mess}')

def xitem(
    x:int,
    val:str,
    tag: str = None,
    color: Color = Color.DEFAULT,
) -> None:
    print(f'{x:>2d}:', end=' ')
    if tag: print(
        color, 
        f'[{tag}]',
        Color.DEFAULT,
        sep='', end=' ',
    )
    print(val)

