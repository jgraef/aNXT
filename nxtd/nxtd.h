/*
    nxtd.h
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

#ifndef _NXTD_H_
#define _NXTD_H_

#include <pthread.h>

/// Max. number of NXTs
#define NXTD_MAXNUM 256

/// NXT ID (unique for ALL NXTs)
typedef char nxtd_id_t[6];

/// Enumeration of connection types
typedef enum {
  /// Connection over Universal Serial Bus
  NXTD_USB,
  /// Connection over Bluetooth
  NXTD_BT
} nxtd_conn_t;

/// Descriptor for NXTs
struct nxtd_nxt {
  /// The NXTs name
  char *name;
  /// ID
  nxtd_id_t id;
  /// How the NXT is connected
  nxtd_conn_t conn_type;
  /// UNIX timestamp when connection times out.
  /// 0 if no connection timeout is set
  time_t conn_timeout;
};

/// List of NXTs
struct {
  /// Mutex for list
  pthread_mutex_t mutex;
  /// List of NXTs
  struct nxtd_nxt *list[NXTD_MAXNUM];
} nxts;

int nxtd_nxt_reg(struct nxtd_nxt *nxt);
struct nxtd_nxt *nxtd_nxt_find(const char *name,nxtd_conn_t conn_type);

#endif /* _NXTD_H_ */
