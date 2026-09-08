from ..supports import SupportsStr, SupportsJSON
from typing import TYPE_CHECKING, TypedDict
from time import perf_counter

if TYPE_CHECKING:
    from ...json import Dict

class CachedItem[T](TypedDict):
    created: float
    value: T

class TransitoryCache[T]:

    def __init__(self, 
        id: SupportsStr = 0, 
        expire: int = 18_000
    ) -> None:
        from ...pc import loc

        self.expire = expire

        file = loc.cache.child(f'TransitoryCache-{id}.pkl')
        self.clear = file.delete
        self._pkl = file.PKL

    @property
    def _dict(self) -> 'Dict[CachedItem]':

        data: dict[str, CachedItem] = self._pkl.read() or {}

        now = perf_counter()

        for key, item in data.copy().items():
            if (now - item['created']) >= self.expire:
                del data[key]

        self._pkl.save(data)

        return self._pkl.Dict

    def __getitem__(self, key:SupportsJSON) -> T:
        return self._dict.read() [key] ['value']
    
    def __setitem__(self, key:SupportsJSON, value:T) -> None:
        self._dict[key] = {
            'created': perf_counter(),
            'value': value
        }

    def __contains__(self, key:SupportsJSON) -> bool:
        return (key in self._dict.read())

