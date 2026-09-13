from inspect import isclass, isfunction
from .printer import Printer

class Branch:

    def __init__(self,
        *args: str
    ) -> None:

        if len(args) == 0:
            cmd = self._NoArgs
        else:
            cmd = getattr(self, args[0].lower(), None)
        

        if isfunction(cmd):
            cmd(*args[1:])

        elif isclass(cmd):
            Branch.__init__(cmd(), *args[1:])

        elif isinstance(cmd, str):
            print(cmd)

        else:
            print("\nINVALID COMMAND\nType 'help' for a list of commands\n")

    @staticmethod
    def _NoArgs() -> None:
        Printer.Error('NoArgs', 'This command requires arguements')

