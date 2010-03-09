# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

.PHONY: all clean

all: ../lib/libanxt_net.a

clean:
	rm -f *.o ../lib/libanxt_net.a ../lib/libanxt_net.so.*

../lib/libanxt_net.a: net.o client.o server.o
	$(AR) rs $@ $^
	$(CC) -shared -Wl,-soname,libanxt_net.so.1 -o ../lib/libanxt_net.so.1 $^ -lc

net.o: net.c
	$(CC) $(CFLAGS) -c -o $@ $<

client.o: client.c
	$(CC) $(CFLAGS) -c -o $@ $<

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $<
