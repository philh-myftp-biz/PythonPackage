from functools import cached_property
from dataclasses import dataclass
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from ..torrent import Torrent
    from ...time import from_stamp

@dataclass(kw_only=True)
class MediaData:
    
    Title: str
    Torrent: 'None|Torrent' = None
    Released: 'None|from_stamp' = None
    imdb_id: None|str = None

    @cached_property
    def tmdb_id(self) -> None|str:
        from ..Omdb import _get_tmdb

        response = _get_tmdb(
            path = f'/find/{self.imdb_id}', 
            external_source = 'imdb_id'
        )

        results: list[dict] = (response.get('tv_results') or response.get('movie_results'))
        if results:
            return results[0]['id']

    @cached_property
    def Year(self) -> None|int:
        return self.Released and self.Released.year

@dataclass
class MovieData(MediaData):
    ...

@dataclass
class ShowData(MediaData):
    Seasons: dict[str, dict[str, 'EpisodeData']]

@dataclass
class EpisodeData(MediaData):
    Number: int
