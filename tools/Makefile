# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include ../Makefile.config

TOOLS = ../bin/nxt_info \
	../bin/nxt_list \
	../bin/nxt_upload \
	../bin/nxt_download \
	../bin/nxt_remove \
	../bin/nxt_beep \
	../bin/nxt_motor \
	../bin/nxt_tacho \
	../bin/nxt_sensor \
	../bin/nxt_run \
	../bin/nxt_pollcmd \
	../bin/nxt_sensorus \
	../bin/nxt_stop \
	../bin/nxt_getprog \
	../bin/nxt_delflash \
	../bin/nxt_up_run \
	../bin/nxt_send \
	../bin/nxt_recv \
	../bin/nxt_resetbt \
	../bin/nxt_lsmod \
	../bin/nxt_setname \
	../bin/nxt_screenshot \
	../bin/nxt_turnoff \
	../bin/nxt_setbutton \
	../bin/nxt_motor_record \
	../bin/nxt_motor_playback \
	../bin/nxt_server \
	../bin/nxt_error \
	../bin/nxt_scan \
	../bin/nxt_mount \
	../bin/nxt_ricc \
	../bin/nxt_rsoc \
	../bin/nxt_rmdc \
	../bin/nxt_calibrate \
	../bin/nxt_sensorgraph \
	../bin/nxt_modfs \
	../bin/nxt_pilot \
	../bin/nxt_reset_tacho \
	../bin/nxt_motor_travel \
	../bin/anxt-config \
	../bin/nxt_joystick \
	../bin/nxt_joystick_travel \
	../bin/nxt_graphic_test \
	../bin/nxt_echo

all: $(TOOLS)

../lib/libanxt.a:
	make -C .. lib/libanxt.a

../lib/libanxttools.a:
	make -C .. lib/libanxttools.a

../lib/libanxtfile.a:
	make -C .. lib/libanxtfile.a

../bin/nxt_info: info.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_list: list.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_upload: upload.c  ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_download: download.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_remove: remove.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_beep: beep.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_motor: motor.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_tacho: tacho.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_sensor: sensor.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_run: run.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_pollcmd: pollcmd.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_sensorus: sensorus.c  ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_stop: stop.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_getprog: getprog.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_delflash: delflash.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_up_run: up_run.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_send: send.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_recv: recv.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_resetbt: resetbt.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_lsmod: lsmod.c ../lib/libanxttools.a ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_setname: setname.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_screenshot: screenshot.c  ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS) -lgd #-lpng -ljpeg

../bin/nxt_turnoff: turnoff.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_setbutton: setbutton.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_motor_record: motor_record.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_motor_playback: motor_playback.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS) 

../bin/nxt_server: server.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS) -lgd #-lpng -ljpeg

../bin/nxt_error: error.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_scan: scan.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_mount: mount.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS) `pkg-config fuse --cflags --libs`

../bin/nxt_ricc: ricc.c ../lib/libanxt.a  ../lib/libanxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libanxtfile.a -lgd

../bin/nxt_rsoc: rsoc.c ../lib/libanxt.a  ../lib/libanxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libanxtfile.a -lsndfile

../bin/nxt_rmdc: rmdc.c ../lib/libanxt.a  ../lib/libanxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libanxtfile.a

../bin/nxt_calibrate: calibrate.c ../lib/libanxt.a ../lib/libanxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libanxtfile.a

../bin/nxt_sensorgraph: sensorgraph.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) `sdl-config --cflags` `sdl-config --libs` ../lib/libanxttools.a

../bin/nxt_modfs: modfs.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) `pkg-config fuse --cflags --libs`

../bin/nxt_reset_tacho: reset_tacho.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_motor_travel: motor_travel.c ../lib/libanxt.a ../lib/libanxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libanxttools.a $(LIBS)

../bin/nxt_pilot: pilot.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< `sdl-config --cflags` `sdl-config --libs` -lSDL_image $(LIBS)

../bin/anxt-config: anxt-config-creator.sh ../Makefile.config
	sh anxt-config-creator.sh > $@
	chmod 755 $@

../bin/nxt_joystick: joystick.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< `sdl-config --cflags` `sdl-config --libs` $(LIBS)

../bin/nxt_joystick_travel: joystick_travel.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< `sdl-config --cflags` `sdl-config --libs` $(LIBS)

../bin/nxt_graphic_test: graphic_test.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< `sdl-config --cflags` `sdl-config --libs` $(LIBS)

../bin/nxt_echo: echo.c ../lib/libanxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TOOLS)
