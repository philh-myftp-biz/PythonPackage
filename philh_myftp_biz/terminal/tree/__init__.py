from requests.structures import CaseInsensitiveDict as cdict
from .branch import get_branches
from .tree import Tree
from . import Printer

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

