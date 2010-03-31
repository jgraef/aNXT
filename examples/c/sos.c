/*
    sos.c - example program to blink the SOS morsecode with the lightsensor
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

#include <unistd.h>
#include <stdio.h>

#include <anxt/nxt.h>

#define FREQ      1000
#define PAUSE     200
#define DUR_SHORT 150
#define DUR_LONG  400

int main(int argc,char *argv[]) {
  int i;
  int sensor = NXT_SENSOR1; // Sensor port at which the light sensor is connected

  nxt_t *nxt = nxt_open(NULL); // Opens a NXT handle. 'NULL' means that we do
                               // not care about which NXT to open (good if
                               // you have only one NXT)
  if (nxt==NULL) { // Don't forget to check if you find a NXT
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  for (i=0;i<3;i++) { // Three times short
    nxt_beep(nxt,FREQ,DUR_SHORT); // play tone
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_ACTIVE,0); // turn light on
    usleep(DUR_SHORT*1000); // wait for short duration
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_INACTIVE,0); // turn light off
    usleep(PAUSE*1000); // wait short pause
  }

  for (i=0;i<3;i++) { // Three times long
    nxt_beep(nxt,FREQ,DUR_LONG); // play tone
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_ACTIVE,0); // turn light on
    usleep(DUR_LONG*1000); // wait for long duration
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_INACTIVE,0); // turn light off
    usleep(PAUSE*1000); // wait short pause
  }

  for (i=0;i<3;i++) { // Three times short
    nxt_beep(nxt,FREQ,DUR_SHORT); // play tone
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_ACTIVE,0); // turn light on
    usleep(DUR_SHORT*1000); // wait for short duration
    nxt_set_sensor_mode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_INACTIVE,0); // turn light off
    usleep(PAUSE*1000); // wait short pause
  }

  nxt_close(nxt); // Don't forget to close your NXT handle

  return 0;
}

