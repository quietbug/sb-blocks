PREFIX = /usr/local/bin
CFLAGS = -std=c99 -pedantic -Wall -Wextra  -Os ${INCS} ${CPPFLAGS}
CC = gcc

all: mpd net uptime

mpd: 
	$(CC) $(FLAGS) -lmpdclient -o sb-mpd sb-mpd.c

net: 
	$(CC) $(FLAGS) -o sb-net sb-net.c

uptime: 
	$(CC) $(FLAGS) -o sb-uptime sb-uptime.c

install:
	cp sb-mpd $(PREFIX)/sb-mpd
	cp sb-net $(PREFIX)/sb-net
	cp sb-uptime $(PREFIX)/sb-uptime

clean:
	rm sb-mpd sb-net sb-uptime
