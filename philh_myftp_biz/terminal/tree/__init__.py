from .printer import Printer
from .branch import Branch

class Tree(Branch):
    """Base Command Tree"""

    @classmethod
    def cls(cls) -> None:
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

    class help:
        _NoArgs = "Help Message not finished"

def run_tree(tree:type[Tree]) -> None:
    from shlex import split
    from .. import warn

    while True:

        try:

            args: list[str] = split(input('\n\\> '))

            if len(args) == 0:
                continue
            elif args[0] == 'exit':
                break
            else:
                tree(*args)

        except KeyboardInterrupt:
            Printer.Error('KeyboardInterrupt')

        except Exception as e:
            warn(e)

