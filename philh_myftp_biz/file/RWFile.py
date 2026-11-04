from typing import TYPE_CHECKING, Callable, Any
from functools import cached_property

if TYPE_CHECKING:
    from ..pc import Path

class RWFile:

    def __init__(self,
        path: 'Path',
        default: Any = None
    ) -> None:

        self.path    = path

        self.default = default

        # Make the parent dir of the output path
        path.parent.mkdir()

    parsed: Any

    _read: Callable[[], Any]

    save: Callable[[Any], None]
    """Write data to the file"""

    def read(self):
        """Read data from the file"""

        if self.path.exists:
            return self._read()
        
        return self.default

    @property
    def raw(self) -> bytes:

        with self.path.open() as f:
        
            raw: str = f.read()

            return raw.encode('utf-8')
        
    @cached_property
    def Dict(self):
        from ..json import Dict
        return Dict(self)
    
    @cached_property
    def List(self):
        from ..json import List
        return List(self)

class XML(RWFile):
    """.XML File"""

    def _read(self) -> dict:
        from xmltodict import parse

        with self.path.open() as f:

            return parse(f.read())

    def save(self,
        data: dict
    ) -> None:
        from xmltodict import unparse

        with self.path.open('w') as f:

            data = unparse(data, pretty=True)

            f.write(data)

class PKL(RWFile):
    """.PKL File"""

    def _read(self):
        from dill import load
        
        with self.path.open('rb') as f:
            return load(f)

    def save(self,
        value: Any
    ) -> None:
        from dill import dump
        
        with self.path.open(mode='wb') as f:
            dump(obj=value, file=f)

class JSON(RWFile):
    """.JSON File"""

    def _read(self):
        from json import load

        return load(fp=self.path.open())

    def save(self, data: dict) -> None:
        from json import dump

        dump(
            obj = data,
            fp = self.path.open(mode='w'),
            indent = 3
        )

class INI(RWFile):
    """.INI/.PROPERTIES File"""
    
    def _read(self):
        from configobj import ConfigObj
        
        return ConfigObj(str(self.path)).dict()
         
    def save(self, data:dict) -> None:
        from configobj import ConfigObj

        obj = ConfigObj(str(self.path))

        for name, value in data.items():
            obj[name] = value

        obj.write()

class YAML(RWFile):
    """.YML/.YAML File"""
    
    def _read(self):
        from yaml import safe_load

        return safe_load(self.raw)
    
    def save(self, data:dict) -> None:
        from yaml import dump

        dump(
            data = data, 
            stream = self.path.open(mode='w'),
            default_flow_style = False,
            sort_keys = False
        )

class TXT(RWFile):
    """.TXT File"""
    
    def _read(self):
        """Read data from the txt file"""
        return self.path.open(mode='r').read()
    
    def save(self, data:str) -> None:
        """Save data to the txt file"""
        self.path.open(mode='w').write(str(data))

class CSV(RWFile):
    """.CSV File"""

    def _read(self):
        from csv import reader

        with self.path.open() as csvfile:
            return reader(csvfile)

    def save(self, data:list[list]) -> None:
        from csv import writer

        with self.path.open('w') as csvfile:
            writer(csvfile).writerows(data)

class TOML(RWFile):
    """.TOML File"""

    def _read(self):
        from toml import load

        with self.path.open() as f:
            return load(f)
        
    def save(self, data:dict) -> None:
        from tomli_w import dump

        with self.path.open('wb') as f:
            dump(data, f, indent=2)

