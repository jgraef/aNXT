/*
    hid.c - example program using HID-Nx sensor
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2009  Janosch Gräf <janosch.graef@gmx.net>

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
#include <anxt/i2c/hid.h>

int main(int argc,char *argv[]) {
  int sensor = NXT_SENSOR1; // Sensor port at which the light sensor is connected

  nxt_t *nxt = nxt_open(NULL); // Opens a NXT handle. 'NULL' means that we do
                               // not care about which NXT to open (good if
                               // you have only one NXT)
  if (nxt==NULL) { // Don't forget to check if you find a NXT
    fprintf(stderr, "Could not find NXT\n");
    return 1;
  }

  // Set sensor mode
  nxt_setsensormode(nxt, sensor, NXT_SENSOR_TYPE_LOWSPEED, NXT_SENSOR_MODE_RAW);
  nxt_wait_after_communication_command();

  // Select ASCII mode
  nxt_hid_set_mode(nxt, sensor, NXT_HID_MODE_ASCII);
  nxt_wait_after_communication_command();

  // Wait 5 seconds to select a text field
  printf("Waiting 5 sec. Select an editable text field\n");
  sleep(5);

  // Send string
  nxt_hid_send_str(nxt, sensor, "Hello World!");

  // Select direct mode
  nxt_hid_set_mode(nxt, sensor, NXT_HID_MODE_DIRECT);
  nxt_wait_after_communication_command();

  // Send newline
  nxt_hid_set_key(nxt, sensor, NXT_HID_KEY_RETURN);
  nxt_wait_after_communication_command();
  nxt_hid_transmit(nxt, sensor);

  nxt_close(nxt); // Don't forget to close your NXT handle

  return 0;
}
