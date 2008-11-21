# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

all: blink melody sos
	make -C server/

blink: blink.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

melody: melody.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

sos: sos.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f blink melody sos
	make -C server/ clean

