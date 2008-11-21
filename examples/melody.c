/*
    melody.c - example program to play the german melody "Alle meine Entchen"
    libnxt - A C library for using LEGO Mindstorms NXT
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

#define DUR1   400
#define DUR2   500
#define PAUSE  150

#define TONE_C 264
#define TONE_D 297
#define TONE_E 330
#define TONE_F 352
#define TONE_G 396
#define TONE_A 440

nxt_t *nxt; // NXT handle

void playtone(int tone,int dur) {
  nxt_beep(nxt,tone,dur); // play tone on NXT
  usleep((dur+PAUSE)*1000); // wait duration of tone plus a little pause
}

int main(int argc,char *argv[]) {
  int i;

  nxt_init(); // Initializes libnxt
  nxt = nxt_open(NULL); // Opens a NXT handle. 'NULL' means that we do
                               // not care about which NXT to open (good if
                               // you have only one NXT)
  if (nxt==NULL) { // Don't forget to check if you find a NXT
    fprintf(stderr,"Could not find NXT\n");
    return 1;
  }

  // "Alle meine Entchen" (German)
  playtone(TONE_C,DUR1); // Al -
  playtone(TONE_D,DUR1); // le
  playtone(TONE_E,DUR1); // mei -
  playtone(TONE_F,DUR1); // ne
  playtone(TONE_G,DUR2); // Ent -
  playtone(TONE_G,DUR2); // chen
  for (i=0;i<2;i++) { // loop two times
    playtone(TONE_A,DUR1); // schwimm -
    playtone(TONE_A,DUR1); // en
    playtone(TONE_A,DUR1); // auf
    playtone(TONE_A,DUR1); // den
    playtone(TONE_G,DUR2); // See
  }
  playtone(TONE_F,DUR1); // Koepf -
  playtone(TONE_F,DUR1); // chen
  playtone(TONE_F,DUR1); // in
  playtone(TONE_F,DUR1); // das
  playtone(TONE_E,DUR2); // Wass -
  playtone(TONE_E,DUR2); // er
  playtone(TONE_D,DUR1); // Schwaenz -
  playtone(TONE_D,DUR1); // chen
  playtone(TONE_D,DUR1); // in
  playtone(TONE_D,DUR1); // die
  playtone(TONE_C,DUR2); // Hoeh'

  nxt_close(nxt); // Don't forget to close your NXT handle

  return 0;
}

