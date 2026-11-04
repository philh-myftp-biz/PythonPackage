from typing import Callable, Any, Self, Iterable, overload
from .Collection import Collection
from random import choice, shuffle
from itertools import chain

class List[V](Collection[V, list[V]]):

    type Func = Callable[[V], Any]

    #=======================================

    def pop(self, i:int=-1, n:int=1) -> tuple[V, ...]:
        n = min(n, len(self.data))
        return tuple(self.data.pop(i) for _ in range(n))

    def random(self) -> None | V:
        if len(self.data) > 0:
            return choice(self.data)

    def max(self, func:Func=lambda x: x) -> None | V:
        if len(self.data) > 0:
            return max(self.read(), key=func)
        return None
    
    #=======================================

    def extend(self, items:Iterable[V]) -> None:
        self.data.extend(items)

    def sort(self, func:Func=lambda x: x) -> None:
        self.data = sorted(self.data, key=func)
    
    def filter(self, func:Func=lambda x: x) -> None:
        self.data = list(filter(func, self.data))

    def flatten(self) -> None:
        self.data = list(chain.from_iterable(self.data))
    
    def reverse(self) -> None:
        self.data.reverse()
    
    def shuffle(self) -> None:
        shuffle(self.data)

    def uniquify(self, func:Func=lambda x: x) -> None:
        data: dict[Any, V] = {}
        for item in self.data:
            data[func(item)] = item
        self.data = list(data.values())

    #=======================================

    def __iadd__(self, value:V) -> Self:
        self.data += value
        return self
    
    def __isub__(self, value:V) -> Self:
        self.data -= value
        return self

    @overload
    def __getitem__(self, key: int) -> V: ...
    @overload
    def __getitem__(self, key: slice) -> list[V]: ...

    def __getitem__(self, key: int | slice) -> V | list[V]:
        return self.data[key]

