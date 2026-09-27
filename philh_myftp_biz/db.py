
MIMETYPES: dict[str, str]

def __getattr__(attr:str):
    from .web.url import URL
    from logger2 import VERBOSE

    if attr == 'MIMETYPES':
        try:
            VERBOSE.pause()
            return URL(
                'https://raw.githubusercontent.com/MineFartS/FileTypes/refs/heads/master/compiled.json',
                max_age = 259200 # 3 days
            ).json
        finally:
            VERBOSE.resume()

    raise AttributeError(f"module '{__name__}' has no attribute '{attr}'")

