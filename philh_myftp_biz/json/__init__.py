
__all__ = [
    "SupportsJSON",
    "load",
    "loads",
    "dump",
    "dumps",
    "List",
    "Dict",
    "Weights",
    "is_json",
]

from ..functools.supports import SupportsJSON
from json import load, loads, dump, dumps
from .List import List
from .Dict import Dict
from .weights import Weights

def is_json(value:str) -> bool:
    """Check if a string contains valid json data"""
    from json.decoder import JSONDecodeError

    try:
        loads(value)
        return True
    except JSONDecodeError:
        return False

