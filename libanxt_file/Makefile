# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

.PHONY: all clean

all: ../lib/libanxt_file.a

clean:
	rm -f *.o ../lib/libanxt_file.a ../lib/libanxt_file.so.*

../lib/libanxt_file.a: cal.o ric.o rmd.o rso.o nvconfig.o
	$(AR) rs $@ $^
	$(CC) -shared -Wl,-soname,libanxt_file.so.1 -o ../lib/libanxt_file.so.1 $^ -lc

cal.o: cal.c
	$(CC) $(CFLAGS) -c -o $@ $<

ric.o: ric.c
	$(CC) $(CFLAGS) -c -o $@ $<

rmd.o: rmd.c
	$(CC) $(CFLAGS) -c -o $@ $<

rso.o: rso.c
	$(CC) $(CFLAGS) -c -o $@ $<

nvconfig.o: nvconfig.c
	$(CC) $(CFLAGS) -c -o $@ $<
