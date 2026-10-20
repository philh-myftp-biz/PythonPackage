import os

_listdir = os.listdir

def listdir(path="."):
    try:
        return _listdir(path)
    except OSError as e:
        if getattr(e, 'winerror', None) == 415:
            raise FileNotFoundError()
        raise

os.listdir = listdir
