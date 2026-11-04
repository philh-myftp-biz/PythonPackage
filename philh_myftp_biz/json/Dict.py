from typing import ItemsView, KeysView, ValuesView, Callable
from .Collection import Collection

class Dict[K,V](Collection[V, dict[K, V]]):

    #=======================================

    def items(self) -> ItemsView[K, V]:
        return self.data.items()

    def keys(self) -> KeysView[K]:
        return self.data.keys()

    def values(self) -> ValuesView[V]:
        return self.data.values()

    def get(self, key:K, default:V=None) -> V:
        return self.data.get(key, default)

    def update(self, other:dict[K, V]) -> None:
        self.data.update(other)

    def filter(self, func:Callable[[K, V], bool]) -> None:
        self.data = {k:v for k,v in self.data.items() if func(k, v)}

    #=======================================

    def __getitem__(self, key:K) -> V:
        return self.data[key]

    #=======================================

