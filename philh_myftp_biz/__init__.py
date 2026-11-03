from os import environ

if not environ.get('STUBGEN'):
    from logger2._log import VERBOSE # TODO: Temporary Backwards Compatibility
    from logger2 import setup
    setup()

