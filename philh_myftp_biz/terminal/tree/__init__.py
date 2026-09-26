from inspect import isclass, isfunction, ismethod, getmembers, getdoc
from requests.structures import CaseInsensitiveDict as cdict
from . import Printer

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

def get_branches[T](cls:type[T]):

    items: cdict = cdict()

    inst: T = cls.__new__(cls)

    for key, val in getmembers(cls):

        if key.startswith('_'): 
            pass

        elif isfunction(val):
            items[key] = val

        elif ismethod(val):
            items[key] = val.__get__(inst, cls)

        elif isclass(val):
            items[key] = get_branches(val)

    return items

def run_tree(tree:type[Tree]) -> None:
    from shlex import split
    from .. import warn

    treemap = get_branches(tree)

    while True:

        try:

            args: list[str] = split(input('\n\\> '))

            if len(args) == 0:
                continue
            elif args[0] == 'exit':
                break
            else:
                branch = treemap
                while isinstance(branch, cdict) and args:
                    branch = branch.get(args.pop(0))
                branch(*args)

        except TypeError:
            Printer.Error("SyntaxError")

        except KeyboardInterrupt:
            Printer.Error('KeyboardInterrupt')

        except Exception as e:
            warn(e)

