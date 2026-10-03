
try:
    from logger2._log import VERBOSE # TODO: Temporary Backwards Compatibility
    from logger2 import setup
    setup()
except ImportError:
    pass

