from inspect import isclass, isfunction, ismethod, getmembers
from requests.structures import CaseInsensitiveDict as cdict

def get_branches[T](cls:type[T], recurse:bool):

    items: cdict = cdict()

    inst: list[T] = []

    for key, val in getmembers(cls):

        if key.startswith('_'): 
            pass

        elif isfunction(val):
            items[key] = val

        elif ismethod(val):
            if not inst: inst += [cls()]
            items[key] = getattr(inst[0], key)

        elif isclass(val):
            items[key] = get_branches(val, True) if recurse else val

    return items

