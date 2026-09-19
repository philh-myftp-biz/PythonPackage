from ._time import Stopwatch, Timeout, TimeStamp

from_stamp = TimeStamp # TODO deprecated

#==============================================================================
from functools import partial as _partial
from pytz import timezone as _timezone

tzinfo = _partial(_timezone, zone="America/New_York")
#==============================================================================

def now() -> TimeStamp:
    """Get details of the current time"""
    from time import time

    return TimeStamp(time())

def from_string(string: str) -> TimeStamp:
    """Get details of time string"""
    from dateutil.parser._parser import ParserError
    from dateutil import parser

    try:
        dt = parser.parse(string)
        return TimeStamp(dt.timestamp())
    except (OSError, ParserError):
        raise TypeError()

def from_ymdhms(
    year:   int = 0,
    month:  int = 1,
    day:    int = 1,
    hour:   int = 0,
    minute: int = 0,
    second: int = 0,
) -> TimeStamp:
    """Get details of time from year, month, day, hour, minute, & second"""
    from datetime import datetime

    t = datetime(
        year=year,
        month=month,
        day=day,
        hour=hour,
        minute=minute,
        second=second,
        tzinfo = tzinfo()
    )

    try:
        return TimeStamp(t.timestamp())    
    except OSError:
        raise TypeError()

#==============================================================================

def sleep(
    s: int,
    show: bool = False
):
    """
    Wrapper for time.Sleep function

    If show is True, then '#/# seconds' will print to the console each second
    """
    from ..terminal import ProgressBar
    from time import sleep

    # If show is True
    if show:

        pbar = ProgressBar(s)
    
        # loop once for each second
        for _ in range(s):

            sleep(1)

            pbar.step()

        pbar.stop()

    else:
        sleep(s)
    
    return True

#==============================================================================

