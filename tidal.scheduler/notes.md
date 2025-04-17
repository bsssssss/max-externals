# The right time

The main challenge in the making of this scheduler is to get the timescales
right.

Tidal sends OSC messages bundled with a timestamp

This timestamp is a 64 bits number that represents the number of seconds elapsed
since 1 January 1900. The first 32 bits are the seconds and the last 32 bits are
the fraction of a second.

It represents a time (as in a date) in the future to 'play' the message at.

So to schedule the message, we need to know how much time there is between the
moment the message is received and the timestamp in the future.

$ delay = time_in_future - current_time $

In C, the closest we can get to the timestamp's timescale is the UNIX time,
which tells us the number of seconds elapsed since 01 January 1970.

To convert this time to epoch 1900 we need to know how much seconds there is
in 70 years: 

    2208988800 seconds (counting leap years !)

The convertion is a simple addition
    
    NTP_time = UNIX_time + 2208988800


