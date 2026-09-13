from sys import modules

if "mypy" in modules:
    URL, Session, Adapter, RetryStrat = [object]*4
else:
    from .url import URL # pyright: ignore[reportUnusedImport]
    from .session import Session, Adapter, RetryStrat # pyright: ignore[reportUnusedImport]

from ._web import FirewallException, IP # pyright: ignore[reportUnusedImport]

# @dead-code-ignore
class Port:
    """Details of a port on a network device"""

    def __init__(self,
        port: int,
        host: str = '127.0.0.1'
    ) -> None:
        
        self.port: int = port

        self.addr: tuple[str, int] = (host, port)

    @property
    def listening(self) -> bool:
        """Check if Port is listening/in use"""

        from socket import error, SHUT_RDWR
        from quicksocketpy import socket

        sock = socket()

        try:
            
            sock.connect(self.addr)
            sock.shutdown(SHUT_RDWR)
            
            sock.close()

            return True

        except error:

            sock.close()
            return False

    def __int__(self) -> int:
        return self.port
    
    def __repr__(self) -> str:
        return f"Port({self.port})"
