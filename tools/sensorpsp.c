/*
    tools/sensorpsp.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <anxt/nxt.h>
#include <anxt/i2c/psp.h>
#include <anxt/tools.h>

void usage(char *cmd,int r) {
  FILE *out = r==0?stdout:stderr;
  fprintf(out,"Usage: %s [OPTION]...\n",cmd);
  fprintf(out,"Show pressed buttons and joystick values of a Playstation Controller (PSP-Nx)\n");
  fprintf(out,"Options:\n");
  fprintf(out,"\t-h         Show help\n");
  fprintf(out,"\t-n NXTNAME Name of NXT (Default: first found) or bluetooth address\n");
  fprintf(out,"\t-s SENSOR  Specify sensor port (Default: 4)\n");
  fprintf(out,"\t-r         Reset sensor after reading\n");
  fprintf(out,"\t-q         Quit mode\n");
  exit(r);
}

int main(int argc,char *argv[]) {
  char *name = NULL;
  int c,newport;
  int port = 3;
  int verbose = 1;
  int reset = 0;
  struct nxt_psp_buttons buttons;
  struct nxt_psp_joystick joysticks[2];

  while ((c = getopt(argc,argv,":hn:s:qr"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'n':
        name = strdup(optarg);
        break;
      case 'q':
        verbose = 0;
        break;
      case 'r':
        reset = 1;
        break;
      case 's':
        newport = atoi(optarg)-1;
        if (NXT_VALID_SENSOR(newport)) port = newport;
        else {
          fprintf(stderr,"Invalid sensor: %s\n",optarg);
          usage(argv[0],1);
        }
        break;
      case ':':
        fprintf(stderr,"Option -%c requires an operand\n",optopt);
        usage(argv[0],1);
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  nxt_t *nxt = nxt_open(name);
  if (nxt==NULL) {
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  nxt_set_sensor_mode(nxt,port,NXT_SENSOR_TYPE_LOWSPEED,NXT_SENSOR_MODE_RAW);
  nxt_wait_after_communication_command();

  // get joystick values
  if (nxt_psp_get_joystick(nxt, port, NXT_PSP_JOY_LEFT|NXT_PSP_JOY_RIGHT, joysticks)==-1) {
    fprintf(stderr, "Error: %s\n", nxt_strerror(nxt_error(nxt)));
    memset(joysticks, 0, sizeof(joysticks));
  }
  nxt_wait_after_direct_command();

  // get button states
  if (nxt_psp_get_buttons(nxt, port, &buttons)==-1) {
    fprintf(stderr, "Error: %s\n", nxt_strerror(nxt_error(nxt)));
    memset(&buttons, 0, sizeof(buttons));
  }
  nxt_wait_after_direct_command();

  if (verbose) {
    printf("Sensor %d:\n", port+1);
    printf("Joystick Left:   (%d %d) %s\n", joysticks[0].x, joysticks[0].y, NXT_BUTTON_STATUS_STR(buttons.l3));
    printf("Joystick Right:  (%d %d) %s\n", joysticks[1].x, joysticks[1].y, NXT_BUTTON_STATUS_STR(buttons.r3));
    printf("Button Left:     %s\n", NXT_BUTTON_STATUS_STR(buttons.left));
    printf("Button Right:    %s\n", NXT_BUTTON_STATUS_STR(buttons.right));
    printf("Button Up:       %s\n", NXT_BUTTON_STATUS_STR(buttons.up));
    printf("Button Down:     %s\n", NXT_BUTTON_STATUS_STR(buttons.down));
    printf("Button X:        %s\n", NXT_BUTTON_STATUS_STR(buttons.x));
    printf("Button Square:   %s\n", NXT_BUTTON_STATUS_STR(buttons.square));
    printf("Button Triangle: %s\n", NXT_BUTTON_STATUS_STR(buttons.triangle));
    printf("Button O:        %s\n", NXT_BUTTON_STATUS_STR(buttons.o));
    printf("L1:              %s\n", NXT_BUTTON_STATUS_STR(buttons.l1));
    printf("L2:              %s\n", NXT_BUTTON_STATUS_STR(buttons.l2));
    printf("R1:              %s\n", NXT_BUTTON_STATUS_STR(buttons.r1));
    printf("R2:              %s\n", NXT_BUTTON_STATUS_STR(buttons.r2));

  }
  else {
    printf("%d %d %d\n", joysticks[0].x, joysticks[0].y, NXT_BUTTON_STATUS_INT(buttons.l3));
    printf("%d %d %d\n", joysticks[1].x, joysticks[1].y, NXT_BUTTON_STATUS_INT(buttons.r3));
    printf("%d %d %d %d\n", NXT_BUTTON_STATUS_INT(buttons.left),
                            NXT_BUTTON_STATUS_INT(buttons.right),
                            NXT_BUTTON_STATUS_INT(buttons.up),
                            NXT_BUTTON_STATUS_INT(buttons.down));
    printf("%d %d %d %d\n", NXT_BUTTON_STATUS_INT(buttons.x),
                            NXT_BUTTON_STATUS_INT(buttons.square),
                            NXT_BUTTON_STATUS_INT(buttons.triangle),
                            NXT_BUTTON_STATUS_INT(buttons.o));
    printf("%d %d %d %d\n", NXT_BUTTON_STATUS_INT(buttons.l1),
                            NXT_BUTTON_STATUS_INT(buttons.l2),
                            NXT_BUTTON_STATUS_INT(buttons.r1),
                            NXT_BUTTON_STATUS_INT(buttons.r2));
  }

  if (reset) {
    nxt_set_sensor_mode(nxt,port,NXT_SENSOR_TYPE_NONE,NXT_SENSOR_MODE_RAW);
  }

  int ret = nxt_error(nxt);
  if (name!=NULL) free(name);
  nxt_close(nxt);

  return ret;
}
