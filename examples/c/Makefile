# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../../Makefile.config

all: blink melody sos psp hid

blink: blink.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

melody: melody.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

sos: sos.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

psp: psp.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS) `sdl-config --cflags --libs` -lSDL_gfx

hid: hid.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS) `sdl-config --cflags --libs` -lSDL_gfx


clean:
	rm -f blink melody sos psp hid

