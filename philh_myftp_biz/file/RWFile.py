from functools import cached_property
from dataclasses import dataclass
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from ..pc import Path

@dataclass
class TXT:
    """.TXT File"""
    
    path: 'Path'

    def read(self) -> str:
        """Read data from the file"""
        with self.r as f:
            return f.read()
    
    def save(self, data:str) -> None:
        """Save data to the file"""
        with self.w as f:
            f.write(str(data))

    @property
    def r(self):
        return self.path.open('r')

    @property
    def rb(self):
        return self.path.open('rb')

    @property
    def w(self):
        return self.path.open('w')

    @property
    def wb(self):
        return self.path.open('wb')
        
    @cached_property
    def Dict(self):
        from ..json import Dict
        return Dict(self.read(), self)
    
    @cached_property
    def List(self):
        from ..json import List
        return List(self.read(), self)

class XML[K,V](TXT):
    """.XML File"""

    def read(self) -> dict[K,V]:
        from xmltodict import parse
        return parse( super().read() ) # pyright: ignore[reportReturnType]

    def save(self, data:dict[K,V]) -> None:
        from xmltodict import unparse
        super().save(
            unparse(data, pretty=True)
        )

class PKL[T](TXT):
    """.PKL File"""

    def read(self) -> T:
        from dill import load        
        return load(self.rb)

    def save(self, value:T) -> None:
        from dill import dump        
        dump(file=self.wb, obj=value)

@dataclass
class JSON[T](TXT):
    """.JSON File"""

    indent: int = 3

    def read(self) -> T:
        from json import load
        return load(self.r)

    def save(self, data:T) -> None:
        from json import dump
        dump(
            obj = data,
            fp = self.w,
            indent = self.indent,
        )

class INI[K,V](TXT):
    """.INI/.PROPERTIES File"""

    @property
    def obj(self):
        from configobj import ConfigObj
        return ConfigObj( str(self.path) )
    
    def read(self) -> dict[K,V]:
        return self.obj.dict()
    
    def save(self, data:dict[K,V]) -> None:
        obj = self.obj
        obj.merge(data)
        obj.write()

class YAML[K,V](TXT):
    """.YML/.YAML File"""
    
    def read(self) -> dict[K,V]:
        from yaml import safe_load
        return safe_load(self.r)
    
    def save(self, data:dict[K,V]) -> None:
        from yaml import dump
        dump(
            data = data, 
            stream = self.w,
            default_flow_style = False,
            sort_keys = False
        )

class CSV(TXT):
    """.CSV File"""

    def read(self) -> list[list[str]]:
        from csv import reader
        return list(reader(self.r))

    def save(self, data:list[list[str]]) -> None:
        from csv import writer
        writer(self.w).writerows(data)

@dataclass
class TOML[K,V](TXT):
    """.TOML File"""

    indent: int = 2

    def read(self) -> dict[K,V]:
        from toml import load
        return load(self.r) # pyright: ignore[reportReturnType]
        
    def save(self, data:dict[K,V]) -> None:
        from tomli_w import dump
        dump(
            obj = data,
            fp = self.wb,
            indent = self.indent,
        )

