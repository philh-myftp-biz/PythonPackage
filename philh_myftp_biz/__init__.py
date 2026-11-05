from os import environ

__all__ = [
    "VERBOSE",
]

if not environ.get('STUBGEN'):
    from logger2._log import VERBOSE # TODO: Temporary Backwards Compatibility
    from logger2 import setup
    setup()

