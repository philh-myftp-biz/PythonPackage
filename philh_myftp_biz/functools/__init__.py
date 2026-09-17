from typing import Any, Callable, TYPE_CHECKING

from .SharedBuffer import SharedBuffer # pyright: ignore[reportUnusedImport]
from .attr import attr, attrs, dunders # pyright: ignore[reportUnusedImport]
from .paths import cpath, spath # pyright: ignore[reportUnusedImport]
from .cache import TransitoryCache, cached_property, clear_cache, diskcache # pyright: ignore[reportUnusedImport]
from .force_types import force_in_types, force_out_type # pyright: ignore[reportUnusedImport]
from .supports import *

if TYPE_CHECKING:
    from ..pc import Path

def is_iterable(obj) -> bool:
    """*Ignores strings"""
    
    if isinstance(obj, (str, bytes, bytearray)):
        return False
    
    try:
        iter(obj)
        return True
    except TypeError:
        return False

def single_use(f): # pyright: ignore[reportMissingParameterType]
    """Ignore all but first executions"""
    from functools import wraps

    @wraps(f)
    def wrapper(*args, **kwargs): # pyright: ignore[reportMissingParameterType]
        
        if not wrapper.has_run:
            
            wrapper.has_run = True
            
            return f(*args, **kwargs)
    
    wrapper.has_run = False

    return wrapper

def loc(obj:Any) -> str:
    """Get the hexadecimal location of an instance in memory"""
    return hex(id(obj))

# @dead-code-ignore
def retryfunc(
    tries: int = 3,
    interval: int = 0,
    exc: Exception = Exception
):
    from tenacity import retry, stop_after_attempt, wait_fixed, retry_if_exception_type

    return retry(
        stop = stop_after_attempt(tries),
        wait = wait_fixed(interval),
        retry = retry_if_exception_type(exc)
    )

def singleton[T](
    cls: type[T] | Callable[..., T]
) -> T:
    return cls()

@force_in_types
def remport[T](
    file: 'Path',
    type: type[T] = Any
) -> T:
    """Remotely Import Python File"""
    import importlib.util as iutil
    spec = iutil.spec_from_file_location(file.name, file.path)
    mod = iutil.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod # pyright: ignore[reportReturnType]

def return_type[T](func: Callable[..., T]) -> None | type[T]:
    from inspect import getsource
    import ast

    source = getsource(func)
    tree = ast.parse(source)

    # Find the return node and guess its type
    for node in ast.walk(tree):
        if isinstance(node, ast.Return) and isinstance(node.value, ast.Constant):
            return type(node.value.value) # pyright: ignore[reportReturnType]

