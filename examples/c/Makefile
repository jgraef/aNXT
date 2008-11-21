# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../../Makefile.config

all: blink melody sos

blink: blink.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

melody: melody.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

sos: sos.c
	$(CC) $(CFLAGS) -o $@ $^ -L../../lib $(LIBS)

clean:
	rm -f blink melody sos

