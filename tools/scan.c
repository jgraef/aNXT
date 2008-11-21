/*
    scan.c
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

#include <nxtnet.h>
#include <stdio.h>

void nxtnet_scan(nxtnet_cli_t *cli) {
  size_t i;

  // Search USB
  struct nxtnet_proto_listusb_sc *listusb = nxtnet_cli_listusb(cli);
  printf("USB:\n");
  if (listusb!=NULL) {
    for (i=0;i<listusb->num_items;i++) printf("%s\t%d\n",listusb->nxts[i].name,listusb->nxts[i].nxtid);
  }

  // Search Bluetooth

  struct nxtnet_proto_listbt_sc *listbt = nxtnet_cli_listbt(cli);
  printf("Bluetooth:\n");
  if (listbt!=NULL) {
    for (i=0;i<listbt->num_items;i++) {
      char *btaddr_str = "00:00:00:00:00:00";
      printf("%s\t%d\t%s\n",listbt->nxts[i].name,listbt->nxts[i].nxtid,btaddr_str);
    }
  }
}

int main(int argc,char *argv[]) {
  /// @todo Scan for NXTNET servers, connect to them and call nxtnet_scan
  return 0;
}
