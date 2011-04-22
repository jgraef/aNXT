# This makefile converts the man.1 manpages to PDFs and concatenates them

MANDIR = ../man1
MAN = man

TARGETS = anxt-config.pdf nxt_beep.pdf nxt_calibrate.pdf nxt_delflash.pdf \
          nxt_download.pdf nxt_error.pdf nxt_getprog.pdf nxt_info.pdf \
          nxt_list.pdf nxt_lsmod.pdf nxt_motor.pdf nxt_motor_playback.pdf \
          nxt_motor_record.pdf nxt_motor_travel.pdf nxt_pilot.pdf \
          nxt_pollcmd.pdf nxt_recv.pdf nxt_remove.pdf nxt_resetbt.pdf \
          nxt_ricc.pdf nxt_rmdc.pdf nxt_rsoc.pdf nxt_run.pdf nxt_scan.pdf \
          nxt_screenshot.pdf nxt_send.pdf nxt_sensor.pdf nxt_sensorus.pdf \
          nxt_server.pdf nxt_setbutton.pdf nxt_setname.pdf nxt_stop.pdf \
          nxt_tacho.pdf nxt_turnoff.pdf nxt_upload.pdf nxt_up_run.pdf

OUTPUT_NAME = "aNXT utilities manual.pdf"

.PHONY: all clean

all: $(TARGETS)
	gs -q -sPAPERSIZE=letter -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=$(OUTPUT_NAME) $(TARGETS)

clean:
	rm -f $(TARGETS)

%.pdf: $(MANDIR)/%.1
	$(MAN) -t $< | ps2pdf - > $@

