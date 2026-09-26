from inspect import getdoc

def _helpseg(cls, key:str, x:tuple[int, int]) -> str:
    attr = getattr(cls, key.lower())
    doc = getdoc(attr) or ""
    doc = "\n".join(doc.splitlines()[x[0]:x[1]])
    return (key.upper() + " | " + doc)

class Tree:
    """Base Command Tree"""

    @classmethod
    def help(cls, *args: str) -> None:
        """Display help message"""

        lines: list[str] = []

        if len(args) > 0:
            lines += [_helpseg(cls, args[0], [0,-1])]
        else:
            for key in dir(cls):
                if key.startswith('_'): continue
                lines += [_helpseg(cls, key, [0,1])]

        #msg += '\n\n' + getdoc(cls)

        for l in lines: print(l)

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

