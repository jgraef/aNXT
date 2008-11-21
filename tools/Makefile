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
	../bin/nxt-config

all: $(TOOLS)

../lib/libnxt.a:
	make -C .. lib/libnxt.a

../lib/libnxttools.a:
	make -C .. lib/libnxttools.a

../lib/libnxtfile.a:
	make -C .. lib/libnxtfile.a

../bin/nxt_info: info.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_list: list.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS)

../bin/nxt_upload: upload.c  ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_download: download.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_remove: remove.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_beep: beep.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_motor: motor.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_tacho: tacho.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_sensor: sensor.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS)

../bin/nxt_run: run.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_pollcmd: pollcmd.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_sensorus: sensorus.c  ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_stop: stop.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_getprog: getprog.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_delflash: delflash.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_up_run: up_run.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_send: send.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_recv: recv.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_resetbt: resetbt.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_lsmod: lsmod.c ../lib/libnxttools.a ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS)

../bin/nxt_setname: setname.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_screenshot: screenshot.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -lgd #-lpng -ljpeg

../bin/nxt_turnoff: turnoff.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_setbutton: setbutton.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_motor_record: motor_record.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS)

../bin/nxt_motor_playback: motor_playback.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS) 

../bin/nxt_server: server.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< ../lib/libnxttools.a $(LIBS) -lgd #-lpng -ljpeg

../bin/nxt_error: error.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_scan: scan.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

../bin/nxt_mount: mount.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) `pkg-config fuse --cflags --libs`

../bin/nxt_ricc: ricc.c ../lib/libnxt.a  ../lib/libnxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libnxtfile.a -lgd

../bin/nxt_rsoc: rsoc.c ../lib/libnxt.a  ../lib/libnxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libnxtfile.a -lsndfile

../bin/nxt_rmdc: rmdc.c ../lib/libnxt.a  ../lib/libnxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libnxtfile.a

../bin/nxt_calibrate: calibrate.c ../lib/libnxt.a ../lib/libnxtfile.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) ../lib/libnxtfile.a

../bin/nxt_sensorgraph: sensorgraph.c ../lib/libnxt.a ../lib/libnxttools.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) `sdl-config --cflags` `sdl-config --libs` ../lib/libnxttools.a

../bin/nxt_modfs: modfs.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) `pkg-config fuse --cflags --libs`

../bin/nxt_pilot: pilot.c ../lib/libnxt.a
	$(CC) $(CFLAGS) -o $@ $< `sdl-config --cflags` `sdl-config --libs` -lSDL_image $(LIBS)

../bin/nxt-config: nxt-config.c
	$(CC) -o $@ $< -DVERSION="\"$(CONFIG_VERSION)\"" -DCFLAGS="\"$(CONFIG_CFLAGS)\"" -DLIBS="\"$(CONFIG_LIBS)\"" -DSTATIC_LIBS="\"$(CONFIG_STATIC_LIBS)\""

clean:
	rm -f $(TOOLS)
