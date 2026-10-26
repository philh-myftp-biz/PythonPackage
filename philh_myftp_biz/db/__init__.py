from enum import StrEnum

from .keyring import Key, Ring

#========================================================

MIMETYPES: dict[str, str]

def __getattr__(attr:str):
    from logger2 import VERBOSE
    from ..web.url import URL

    if attr == 'MIMETYPES':
        try:
            VERBOSE.pause()
            return URL(
                'https://raw.githubusercontent.com/MineFartS/FileTypes/refs/heads/master/compiled.json',
                max_age = 259200 # 3 days
            ).json
        finally:
            VERBOSE.resume()

    raise AttributeError(f"module '{__name__}' has no attribute '{attr}'")

#========================================================

class Color(StrEnum):
    r"""
    COLOR CONVERSION TABLE

    EXAMPLE:
    Color.RED -> '\033[31m'
    """
    BLACK = '\033[30m'
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    BLUE = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN = '\033[36m'
    WHITE = '\033[37m'
    DEFAULT = '\033[0m'
    BOLD = '\033[1m'
    GRAY = '\033[90m'

#========================================================
