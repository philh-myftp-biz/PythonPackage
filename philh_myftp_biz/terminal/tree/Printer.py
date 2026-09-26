from ...functools.force_types import force_in_types

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

def xitem(x:int, val:str) -> None:
    print(f'{x:>2d}:', val)

