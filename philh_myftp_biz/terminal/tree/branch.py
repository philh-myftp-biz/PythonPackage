from inspect import isclass, isfunction, ismethod, getmembers
from requests.structures import CaseInsensitiveDict as cdict

def get_branches[T](cls:type[T], ):

    items: cdict = cdict()

    inst: T = cls.__new__(cls)

    for key, val in getmembers(cls):

        if key.startswith('_'): 
            pass

        elif isfunction(val):
            items[key] = val

        elif ismethod(val):
            items[key] = val.__get__(inst, cls)

        elif isclass(val):
            items[key] = get_branches(val)

    return items

