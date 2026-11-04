from ..supports import SupportsStr, SupportsJSON
from time import perf_counter as now
from typing import TypedDict
from ...pc._pc import loc
from ...json import Dict

class CachedItem[T](TypedDict):
    created: float
    value: T

class TransitoryCache[T]:

    def __init__(self, 
        id: SupportsStr = 0, 
        expire: int = 18_000
    ) -> None:

        self.expire = expire

        file = loc.cache.child(f'TransitoryCache-{id}.pkl')
        self.clear = file.delete
        self.dict: Dict[str, CachedItem[T]] = file.PKL.Dict

    def __getitem__(self, key:SupportsJSON) -> T:
        self.dict.filter(
            lambda k,v: (now() - v['created']) < self.expire
        )
        self.dict.flush()
        return self.dict[key]['value']
    
    def __setitem__(self, key:SupportsJSON, value:T) -> None:
        self.dict[key] = {
            'created': now(),
            'value': value
        }
        self.dict.flush()

    def __contains__(self, key:SupportsJSON) -> bool:
        return (key in self.dict)

