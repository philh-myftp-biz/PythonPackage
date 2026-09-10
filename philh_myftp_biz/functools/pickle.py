from _pickle import PicklingError
from dill import dumps

def _rebuild_instance(cls):
    return cls.__new__(cls)

def is_pickleable(obj):
    try:
        dumps(obj)
        return True
    except (PicklingError, AttributeError):
        return False

class Pickleable:

    def __getstate__(self):
        return {
            k:v for k,v
            in self.__dict__.items()
            if is_pickleable(v)
        }

    def __setstate__(self, state):
        self.__dict__.update(state)

    def __reduce__(self):
        return (
            _rebuild_instance, 
            (self.__class__,),
            self.__getstate__()
        )

