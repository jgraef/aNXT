# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

.PHONY: all clean

all: ../lib/libanxt.a

clean:
	rm -f *.o mkfont font.h ../lib/libanxt.a ../lib/libanxt.so.*

../lib/libanxt.a: sendrecv.o nxt.o display.o file.o i2c.o ls.o mod.o us.o nxtcam.o psp.o accel.o hid.o
	$(AR) rs $@ $^
	$(CC) -shared -Wl,-soname,libanxt.so.1 -o ../lib/libanxt.so.1 $^ -lc

sendrecv.o: sendrecv.c
	$(CC) $(CFLAGS) -c -o $@ $<

nxt.o: nxt.c
	$(CC) $(CFLAGS) -c -o $@ $<

display.o: display.c font.h
	$(CC) $(CFLAGS) -c -o $@ $< -include font.h

file.o: file.c
	$(CC) $(CFLAGS) -c -o $@ $<

i2c.o: i2c.c
	$(CC) $(CFLAGS) -c -o $@ $<

ls.o: ls.c
	$(CC) $(CFLAGS) -c -o $@ $<

mod.o: mod.c
	$(CC) $(CFLAGS) -c -o $@ $<

us.o: us.c
	$(CC) $(CFLAGS) -c -o $@ $<

nxtcam.o: nxtcam.c
	$(CC) $(CFLAGS) -c -o $@ $<

psp.o: psp.c
	$(CC) $(CFLAGS) -c -o $@ $<

accel.o: accel.c
	$(CC) $(CFLAGS) -c -o $@ $<

hid.o: hid.c
	$(CC) $(CFLAGS) -c -o $@ $<

font.h: font_8x5.png mkfont
	./mkfont < $< > $@

mkfont: mkfont.c
	gcc -o $@ $< -lgd -lpng
