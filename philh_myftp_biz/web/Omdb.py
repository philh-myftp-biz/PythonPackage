from .torrent.models import MovieData, ShowData, EpisodeData
from typing import NoReturn, Literal
from .url import URL

#=================================================================
# TMDB (The Movie Database)

_tmdb_key = "eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJjYjg3MDhlNGRhZDkxOTI5N2Y2YWExZmNjN2NkYjMzYyIsIm5iZiI6MTc4MzUxOTM2Mi41MTgwMDAxLCJzdWIiOiI2YTRlNTg4MmNjNDg5MDY0MjJmOTBhOGIiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0.mZvPMjqmMyNlBoY-wJsWQMruep92MwJqIJ35M65gMSI"

_tmdb_url = URL(
    url = "https://api.themoviedb.org/3/",
    max_age = 10800, # 3 hours
    headers = {
        "accept": "application/json",
        "Authorization": f"Bearer {_tmdb_key}"
    }
)

def _get_tmdb(path:str, **params) -> dict:
    try:
        return _tmdb_url.child(path, params=params).json
    except TimeoutError, ConnectionError:
        return {}

#=================================================================
# OMDB (Open Movie Database)

_omdb_key: Literal['dc888719','2e0c4a98'] = 'dc888719'

_omdb_url = URL(
    url = 'https://www.omdbapi.com/',
    max_age = 10800 # 3 hours
)

def _get_omdb(**params) -> NoReturn | dict:

    data = _omdb_url.copy(
        params = {**params, 'apikey':_omdb_key}
    ).json

    error = data.get('Error')

    if error is None:
        return data
    elif 'not found!' in error:
        raise IndexError(error)
    else:
        raise ConnectionAbortedError(error)

#=================================================================

def movie(title:str, year:int) -> None | MovieData:
    """Get details of a movie"""
    from ..time import from_string

    r = _get_omdb(t=title, y=year)

    if bool(r['Response']) and (r['Type'] == 'movie'):
        
        m = MovieData(
            Title = r['Title'],
        )

        try:
            m.Released = from_string(r['Released'])
        except TypeError:
            pass

        return m

def show(
    title: str,
    year: int
) -> None | ShowData:
    """Get details of a show"""
    from ..time import from_string, from_ymdhms
    
    # Request raw list of seasons
    r1 = _get_omdb(t=title, y=year)

    # Create new 'Show' obj
    show = ShowData(
        Seasons = {},
        Title = title,
    )
    
    try:
        show.Released = from_ymdhms(year=year)
    except TypeError:
        pass

    tmdb_id: list[dict] = _get_tmdb(
        path = f'/find/{r1['imdbID']}', 
        external_source = 'imdb_id'
    ) ['tv_results'] [0] ['id']
    
    # Iter through all seasons by #
    for s in range(0, int(r1['totalSeasons'])+1):

        r2: dict = _get_tmdb(f'/tv/{tmdb_id}/season/{s}')

        show.Seasons [f'{s:02d}'] = {}

        # Iterate through the episodes in the season
        for e in r2.get('episodes', []):

            episode = EpisodeData(
                Title = e['name'],
                Number = e['episode_number']
            )
            
            try:
                episode.Released = from_string(e['air_date'])
            except TypeError:
                pass

            show.Seasons [f'{s:02d}'] [f'{episode.Number:02}'] = episode

    return show

