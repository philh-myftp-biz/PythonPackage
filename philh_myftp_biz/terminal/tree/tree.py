from inspect import getdoc, isfunction
from .branch import get_branches

class Tree:
    """Base Command Tree"""

    @classmethod
    def help(cls, *args: str) -> None:
        """Display help message"""

        x: list = [0, 1]
        name: tuple[str] = ()

        # ('help', args[0])
        if len(args) > 0:
            cls = getattr(cls, args[0])
            name = (cls.__name__.upper(),)
            x[1] = None

        if isfunction(cls):
            name = ()
            branches = {cls.__name__: cls}
        else:
            branches = get_branches(cls, False)

        for key, val in branches.items():
            print( 
                *name,
                key.upper(), '|',
                "\n".join((getdoc(val) or "").splitlines()[x[0]:x[1]])
            )

    @classmethod
    def cls(cls) -> None:
        """Clear the terminal"""
        from .. import _cls_cmd
        from os import system
        
        system(_cls_cmd)

        lines = [line.strip() for line in cls.__doc__.strip().split('\n')]

        _len = int( 1.25 * len(max(lines)) )
        divider = ('|' + _len*'-' + '|')

        lines.insert(0, divider)
        lines += [divider]

        for x, line in enumerate(lines):
            lines[x] = line.center(len(divider))
        
        print('\n'.join(lines))

