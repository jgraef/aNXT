# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

.PHONY: all clean

all: ../bin/nxtd

clean:
	rm -f ../bin/nxtd nxtd_usb_libusb.libs

nxtd_usb_libusb.libs:
	pkg-config libusb-1.0 --cflags --libs > $@

../bin/nxtd: nxtd.c nxtd_usb_$(USB_MOD).c nxtd_bt_$(BT_MOD).c ../lib/libanxt_net.a nxtd_usb_$(USB_MOD).libs nxtd_bt_$(BT_MOD).libs
	$(CC) $(CFLAGS) -o $@ $< -L../lib/ -lanxt_net -lpthread \
         -DNXTD_PIDFILE=\"$(NXTD_PIDFILE)\" \
         nxtd_usb_$(USB_MOD).c \
         nxtd_bt_$(BT_MOD).c \
         -include nxtd_usb_$(USB_MOD).h \
         -include nxtd_bt_$(BT_MOD).h \
         `cat nxtd_usb_$(USB_MOD).libs` \
         `cat nxtd_bt_$(BT_MOD).libs`
