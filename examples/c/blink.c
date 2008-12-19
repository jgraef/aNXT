/*
    blink.c - example program to blink with the light sensor
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

#include <nxt.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char *argv[]) {
  int times = 5; // How often should the light sensor blink
  int sensor = NXT_SENSOR1; // Sensor port at which the light sensor is connected
  int i;

  nxt_t *nxt = nxt_open(NULL); // Opens a NXT handle. 'NULL' means that we do
                               // not care about which NXT to open (good if
                               // you have only one NXT)
  if (nxt==NULL) { // Don't forget to check if you find a NXT
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  for (i=0;i<times;i++) {
    nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_ACTIVE,0); // Turn the light on
    usleep(500000); // Wait 0.5 sec
    nxt_setsensormode(nxt,sensor,NXT_SENSOR_TYPE_LIGHT_INACTIVE,0); // Turn the light off
    usleep(500000); // Wait 0.5 sec again
  }

  nxt_close(nxt); // Don't forget to close your NXT handle

  return 0;
}
