# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

.PHONY: all clean

all: ../lib/libanxt_tools.a

clean:
	rm -f *.o ../lib/libanxt_tools.a ../lib/libanxt_tools.so.*

../lib/libanxt_tools.a: tools.o
	$(AR) rs $@ $^
	$(CC) -shared -Wl,-soname,libanxt_tools.so.1 -o ../lib/libanxt_tools.so.1 $^ -lc

tools.o: tools.c
	$(CC) $(CFLAGS) -c -o $@ $<
