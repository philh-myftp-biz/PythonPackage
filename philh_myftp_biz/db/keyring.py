from dataclasses import dataclass

# @dead-code-ignore
class Ring:
    """Wrapper for keyring"""
    
    def __init__(self, name:str) -> None:
        from ..text import hex

        self.rname = name
        self.name: str = 'philh.myftp.biz/' + hex.encode(name)

    def Key(self, name:str) -> 'Key':
        """Get Key in Ring by name"""

        return Key(ring=self, name=name)
    
    __getitem__ = Key

@dataclass
class Key[T]:
    """Wrapper for keyring"""
    
    ring: Ring
    name: str

    def save(self, value:T) -> None:
        """Save value to Key"""
        from keyring import set_password
        from ..text import hex

        set_password(
            service_name = self.ring.name,
            username = hex.encode(self.name),
            password = hex.encode(value=value)            
        )
        
    def read(self) -> None | T:
        """Read value from key"""
        from keyring import get_password
        from ..text import hex

        rvalue: str | None = get_password(
            service_name = self.ring.name,
            username = hex.encode(self.name)
        )
        
        try:
            return hex.decode(rvalue) # pyright: ignore[reportReturnType]
        except TypeError:
            return None
        
    def prompt(self, secure:bool) -> None:
        """Open GUI window asking for value"""
        from ..gui import Window, Widget

        gui = Window()
        gui.title = 'Keyring Prompt'

        page = gui.Page()
        page += Widget.Text(f'Ring: {self.ring.rname}')
        page += Widget.Text(f'Key: {self.name}')
        page += Widget.Input(self.name, key=self, secure=secure)
        page += Widget.Button('Save', gui.close)

        gui.page = page

        gui.run()
