from typing import Self, Any, Iterator
from typing import get_args
from copy import deepcopy
from ..file import TXT
from json import dumps

class Collection[T, STRUCT]:

    data: STRUCT

    def __init__(self,
        t: 'STRUCT | TXT | Collection[T, STRUCT] | Any' = None,
        file: TXT|None = None
    ) -> None:
        
        self.cls = get_args(self.__class__.__orig_bases__[0])[1].__origin__
        
        self.file = file

        if data is None:
            self.clear()
        else:
            self.data = deepcopy(self.cls(t))

    #=======================================
    
    def clear(self) -> None:
        self.data = self.cls()

    def flush(self) -> None:
        """Save data to file"""
        if self.file:
            self.file.save(self.data)
    
    def copy(self) -> Self:
        return self.__class__( deepcopy(self.data) )

    #=======================================

    def __len__(self) -> int:
        return len(self.data)
        
    def __setitem__(self, key:Any, value:T) -> None:
        self.data[key] = value

    def __delitem__(self, key: Any) -> None:
        del self.data[key]

    def __contains__(self, key: Any) -> bool:
        return (key in self.data)
    
    def __str__(self) -> str:
        return dumps(
            obj = self.data,
            indent = 2
        )
    
    def __repr__(self) -> str:
        return str(self.data)

    def __iter__(self) -> Iterator[T]:
        return iter(self.data)

    #=======================================

