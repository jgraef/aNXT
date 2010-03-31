/*
    include/anxt/motor.h
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _ANXT_MOTOR_H_
#define _ANXT_MOTOR_H_

#include <anxt/nxt.h>

// Constants for motor ports
#define NXT_MOTORA  0
#define NXT_MOTORB  1
#define NXT_MOTORC  2
#define NXT_MOTOR_A NXT_MOTORA
#define NXT_MOTOR_B NXT_MOTORB
#define NXT_MOTOR_C NXT_MOTORC
#define NXT_MA      NXT_MOTORA
#define NXT_MB      NXT_MOTORB
#define NXT_MC      NXT_MOTORC

// Turn motor on
#define NXT_MOTOR_ON        1
// Run motor and then brake instead of float
#define NXT_MOTOR_BRAKE     2
// Turn regulation on
#define NXT_MOTOR_REGULATED 4

// No motor regulation
#define NXT_MOTOR_REGMODE_NONE        0
// Regulate motor speed
#define NXT_MOTOR_REGMODE_SPEED       1
// Enable synchronization for this motor
#define NXT_MOTOR_REGMODE_SYNC        2

// Idle motor
#define NXT_MOTOR_RUNSTATE_IDLE     0
// Ramp motor up
#define NXT_MOTOR_RUNSTATE_RAMPUP   0x10
// Run motor
#define NXT_MOTOR_RUNSTATE_RUNNING  0x20
// Ramp motor down
#define NXT_MOTOR_RUNSTATE_RAMPDOWN 0x40

#define NXT_VALID_MOTOR(m)    ((m)>=0 && (m)<=2)
#define NXT_VALID_POWER(p)    ((p)>=-100 && (p)<=100)
#define NXT_VALID_REGMODE(m)  ((m)==NXT_MOTOR_REGMODE_SPEED || (m)==NXT_MOTOR_REGMODE_SYNC)
#define NXT_VALID_RUNSTATE(s) ((s)==NXT_MOTOR_RUNSTATE_IDLE || \
                               (s)==NXT_MOTOR_RUNSTATE_RAMPUP || \
                               (s)==NXT_MOTOR_RUNSTATE_RUNNING || \
                               (s)==NXT_MOTOR_RUNSTATE_RAMPDOWN)

#define nxt_motor_get_tacho(nxt, motor) nxt_motor_get_rotation_count(nxt, motor)

int nxt_motor_reset(nxt_t *nxt, int motor);
int nxt_motor_set_state(nxt_t *nxt, int motor);
int nxt_motor_get_state(nxt_t *nxt, int motor);
int nxt_motor_enable_autoset(nxt_t *nxt, int motor, int enable);
int nxt_motor_enable_autoget(nxt_t *nxt, int motor, int enable);
int nxt_motor_turn_on(nxt_t *nxt, int motor, int on_off);
int nxt_motor_is_turned_on(nxt_t *nxt, int motor);
int nxt_motor_use_brake(nxt_t *nxt, int motor, int on_off);
int nxt_motor_is_using_brake(nxt_t *nxt, int motor);
int nxt_motor_set_regulation(nxt_t *nxt, int motor, int regmode);
int nxt_motor_get_regulation(nxt_t *nxt, int motor);
int nxt_motor_set_power(nxt_t *nxt, int motor, int power);
int nxt_motor_get_power(nxt_t *nxt, int motor);
int nxt_motor_set_turnratio(nxt_t *nxt, int motor, int turnratio);
int nxt_motor_get_turnratio(nxt_t *nxt, int motor);
int nxt_motor_set_runstate(nxt_t *nxt, int motor, int runstate);
int nxt_motor_get_runstate(nxt_t *nxt, int motor);
int nxt_motor_set_rotation(nxt_t *nxt, int motor, int rotation);
int nxt_motor_rotate(nxt_t *nxt, int motor, int rotation, int power);
int nxt_motor_stop(nxt_t *nxt, int motor, int brake);
int nxt_motor_sync(nxt_t *nxt, int motor, int turnratio);
int nxt_motor_reset_tacho(nxt_t *nxt,int motor,int relative);
int nxt_motor_get_tacho_count(nxt_t *nxt, int motor);
int nxt_motor_get_tacho_limit(nxt_t *nxt, int motor);
int nxt_motor_get_tacho_block_count(nxt_t *nxt, int motor);
int nxt_motor_get_rotation_count(nxt_t *nxt, int motor);
int nxt_motor_get_pid(nxt_t *nxt, int motor, int *p, int *i, int *d);
int nxt_motor_set_pid(nxt_t *nxt, int motor, int p, int i, int d);
int nxt_motor_get_speed(nxt_t *nxt, int motor);

/**
 * Convenience function for running motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param power Power to run with
 *  @return Success?
 */
static __inline__ int nxt_motor_run(nxt_t *nxt, int motor, int power) {
  return nxt_motor_rotate(nxt, motor, 0, power);
}

#endif /* _ANXT_MOTOR_H_ */
