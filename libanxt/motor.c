/*
    libanxt/motor.c
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

#include <anxt/nxt.h>
#include <anxt/motor.h>
#include <anxt/mod.h>

#include "private.h"

#include <stdio.h>

/**
 * Resets local state of a motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 *  @note Only to be used by nxt_open_net()
 */
int nxt_motor_reset(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].autoset = 1;
  nxt->motors[motor].autoget = 1;
  nxt->motors[motor].on = 0;
  nxt->motors[motor].brake = 0;
  nxt->motors[motor].power = 0;
  nxt->motors[motor].regmode = NXT_MOTOR_REGMODE_NONE;
  nxt->motors[motor].turnratio = 0;
  nxt->motors[motor].runstate = NXT_MOTOR_RUNSTATE_IDLE;
  nxt->motors[motor].tacho_limit = 0;
  nxt->motors[motor].tacho_count = 0;
  nxt->motors[motor].tacho_block_count = 0;
  nxt->motors[motor].rotation_count = 0;

  return NXT_SUCC;
}

/**
 * Sets state of a motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 */
int nxt_motor_set_state(nxt_t *nxt, int motor) {
  int mode;

  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  mode = (nxt->motors[motor].on?NXT_MOTOR_ON:0)
        |(nxt->motors[motor].brake?NXT_MOTOR_BRAKE:0)
        |(nxt->motors[motor].regmode!=NXT_MOTOR_REGMODE_NONE?NXT_MOTOR_REGULATED:0);

  nxt_pack_start(nxt, 0x04);
  nxt_pack_byte(nxt, motor);
  nxt_pack_byte(nxt, nxt->motors[motor].power);
  nxt_pack_byte(nxt, mode);
  nxt_pack_byte(nxt, nxt->motors[motor].regmode);
  nxt_pack_byte(nxt, nxt->motors[motor].turnratio);
  nxt_pack_byte(nxt, nxt->motors[motor].runstate);
  nxt_pack_dword(nxt,nxt->motors[motor].tacho_limit);

  /*printf("Set Motor %c:\n", motor+'A');
  printf("  Power:       %d%%\n", nxt->motors[motor].power);
  printf("  On:          %s\n", nxt->motors[motor].on?"yes":"no");
  printf("  Brake:       %s\n", nxt->motors[motor].brake?"yes":"no");
  printf("  Regulation:  %d\n", nxt->motors[motor].regmode);
  printf("  Turn ratio:  %d\n", nxt->motors[motor].turnratio);
  printf("  Run state:   %d\n", nxt->motors[motor].runstate);
  printf("  Tacho limit: %d\n", nxt->motors[motor].tacho_limit);*/

  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt, 3));
  test(nxt_unpack_start(nxt, 0x04));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

/**
 * Gets state of a motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 */
int nxt_motor_get_state(nxt_t *nxt, int motor) {
  int mode;

  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_pack_start(nxt, 0x06);
  nxt_pack_byte(nxt, motor);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,25));
  test(nxt_unpack_start(nxt,0x06));
  if (nxt_unpack_error(nxt)==0) {
    nxt_unpack_byte(nxt); // Motor
    nxt->motors[motor].power = nxt_unpack_byte(nxt); // Power
    mode = nxt_unpack_byte(nxt); // Mode
    if (mode&NXT_MOTOR_ON) {
      nxt->motors[motor].on = 1;
    }
    if (mode&NXT_MOTOR_BRAKE) {
      nxt->motors[motor].brake = 1;
    }
    nxt->motors[motor].regmode = nxt_unpack_byte(nxt); // Regulation Mode
    nxt->motors[motor].turnratio = nxt_unpack_byte(nxt); // Turn Ratio
    nxt->motors[motor].runstate = nxt_unpack_byte(nxt); // RunState
    nxt->motors[motor].tacho_limit = nxt_unpack_dword(nxt); // Tacho Limit
    nxt->motors[motor].tacho_count = nxt_unpack_dword(nxt); // Tacho Count
    nxt->motors[motor].tacho_block_count = nxt_unpack_dword(nxt); // Block Tacho Count
    nxt->motors[motor].rotation_count = nxt_unpack_dword(nxt); // Rotation Count

    /*printf("Get Motor %c:\n", motor+'A');
    printf("  Power:       %d%%\n", nxt->motors[motor].power);
    printf("  On:          %s\n", nxt->motors[motor].on?"yes":"no");
    printf("  Brake:       %s\n", nxt->motors[motor].brake?"yes":"no");
    printf("  Regulation:  %d\n", nxt->motors[motor].regmode);
    printf("  Turn ratio:  %d\n", nxt->motors[motor].turnratio);
    printf("  Run state:   %d\n", nxt->motors[motor].runstate);
    printf("  Tacho limit: %d\n", nxt->motors[motor].tacho_limit);*/


    return NXT_SUCC;
  }
  else {
    return NXT_FAIL;
  }
}

/**
 * Set state of motor if autoset is enabled
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 */
static int nxt_motor_autoset(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  if (nxt->motors[motor].autoset) {
    return nxt_motor_set_state(nxt, motor);
  }
  else {
    return NXT_SUCC;
  }
}

/**
 * Get state of motor if autoset is enabled
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 */
static int nxt_motor_autoget(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  if (nxt->motors[motor].autoget) {
    return nxt_motor_get_state(nxt, motor);
  }
  else {
    return NXT_SUCC;
  }
}

/**
 * Enable autoset for motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param enable Whether to enable autoset
 *  @return Success?
 */
int nxt_motor_enable_autoset(nxt_t *nxt, int motor, int enable) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].autoset = enable;
  return NXT_SUCC;
}

/**
 * Enable autoget for motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param enable Whether to enable autoget
 *  @return Success?
 */
int nxt_motor_enable_autoget(nxt_t *nxt, int motor, int enable) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].autoget = enable;
  return NXT_SUCC;
}

/**
 * Turn motor on/off
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param on_off Whether to turn it on (true) or off (false)
 *  @return Success?
 */
int nxt_motor_turn_on(nxt_t *nxt, int motor, int on_off) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].on = on_off;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Return whether motor is turned on
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Whether motor is turned on (true) or off (false)
 */
int nxt_motor_is_turned_on(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return 0;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].on;
}

/**
 * Turn on brake when motor stops
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Success?
 */
int nxt_motor_use_brake(nxt_t *nxt, int motor, int on_off) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].brake = on_off;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Return whether brake is used
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Whether brake is used
 */
int nxt_motor_is_using_brake(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return 0;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].brake;
}

/**
 * Set regulation mode
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param regmode Regulation mode
 *  @return Success?
 */
int nxt_motor_set_regulation(nxt_t *nxt, int motor, int regmode) {
  if (!NXT_VALID_MOTOR(motor) || !NXT_VALID_REGMODE(regmode)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].regmode = regmode;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Get regulation mode
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Regulation mode
 */
int nxt_motor_get_regulation(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_MOTOR_REGMODE_NONE;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].regmode;
}

/**
 * Set motor power
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param power Power (in percent; negative for opposite direction)
 *  @return Success?
 */
int nxt_motor_set_power(nxt_t *nxt, int motor, int power) {
  if (!NXT_VALID_MOTOR(motor) || !NXT_VALID_POWER(motor)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].power = power;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Get motor power
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Power (in percent; negative for opposite direction)
 */
int nxt_motor_get_power(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return 0;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].power;
}

/**
 * Set turn ratio for synchronized motors
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param turnratio Turn ratio
 *  @return Success?
 */
int nxt_motor_set_turnratio(nxt_t *nxt, int motor, int turnratio) {
  if (!NXT_VALID_MOTOR(motor) || !NXT_VALID_POWER(turnratio)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].turnratio = turnratio;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Get turn ratio for synchronized motors
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Turn ratio
 */
int nxt_motor_get_turnratio(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return 0;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].turnratio;
}

/**
 * Set run state
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param runstate Run state
 *  @return Success?
 */
int nxt_motor_set_runstate(nxt_t *nxt, int motor, int runstate) {
  if (!NXT_VALID_MOTOR(motor) || !NXT_VALID_RUNSTATE(runstate)) {
    return NXT_FAIL;
  }

  nxt->motors[motor].runstate = runstate;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Get run state
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @return Run state
 */
int nxt_motor_get_runstate(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_MOTOR_RUNSTATE_IDLE;
  }

  nxt_motor_autoget(nxt, motor);
  return nxt->motors[motor].runstate;
}

/**
 * Set rotation
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param rotation Rotation in degrees (0 for unlimited rotation)
 *  @return Success?
 */
int nxt_motor_set_rotation(nxt_t *nxt, int motor, int rotation) {
  if (!NXT_VALID_MOTOR(motor) || rotation<0) {
    return NXT_FAIL;
  }

  nxt->motors[motor].tacho_limit = rotation;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Convenience function for rotating motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param rotation Rotation in degrees (0 for unlimited rotation)
 *  @param power Power to rotate with
 *  @return Success?
 */
int nxt_motor_rotate(nxt_t *nxt, int motor, int rotation, int power) {
  int autoset;

  autoset = nxt->motors[motor].autoset;
  nxt->motors[motor].autoset = 0;

  nxt_motor_set_power(nxt, motor, power);
  nxt_motor_set_rotation(nxt, motor, rotation);
  nxt_motor_set_runstate(nxt, motor, NXT_MOTOR_RUNSTATE_RUNNING);
  nxt_motor_turn_on(nxt, motor, 1);

  nxt->motors[motor].autoset = autoset;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Convenience function for stopping motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param brake Whether to use brake
 *  @return Success?
 */
int nxt_motor_stop(nxt_t *nxt, int motor, int brake) {
  int autoset;

  autoset = nxt->motors[motor].autoset;
  nxt->motors[motor].autoset = 0;

  nxt_motor_set_power(nxt, motor, 0);
  nxt_motor_use_brake(nxt, motor, brake);
  nxt_motor_set_runstate(nxt, motor, NXT_MOTOR_RUNSTATE_IDLE);
  nxt_motor_turn_on(nxt, motor, 0);

  nxt->motors[motor].autoset = autoset;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Convenience function for synchronizing motor
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param turnratio Turn ratio
 *  @return Success?
 */
int nxt_motor_sync(nxt_t *nxt, int motor, int turnratio) {
  int autoset;

  autoset = nxt->motors[motor].autoset;
  nxt->motors[motor].autoset = 0;

  nxt_motor_set_turnratio(nxt, motor, turnratio);
  nxt_motor_set_regulation(nxt, motor, NXT_MOTOR_REGMODE_SYNC);

  nxt->motors[motor].autoset = autoset;
  return nxt_motor_autoset(nxt, motor);
}

/**
 * Resets current tacho value
 *  @param nxt NXT handle
 *  @param motor Motor
 *  @param relative Relativ (or absolute)
 *  @return Whether resetting was successful
 */
int nxt_motor_reset_tacho(nxt_t *nxt,int motor,int relative) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_pack_start(nxt,0x0A);
  nxt_pack_byte(nxt,motor);
  nxt_pack_byte(nxt,relative?1:0);
  test(nxt_con_send(nxt));
  test(nxt_con_recv(nxt,3));
  test(nxt_unpack_start(nxt,0x0A));
  return nxt_unpack_error(nxt)==0?NXT_SUCC:NXT_FAIL;
}

int nxt_motor_get_tacho_count(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_motor_get_state(nxt, motor);
  return nxt->motors[motor].tacho_count;
}

int nxt_motor_get_tacho_limit(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_motor_get_state(nxt, motor);
  return nxt->motors[motor].tacho_limit;
}

int nxt_motor_get_tacho_block_count(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_motor_get_state(nxt, motor);
  return nxt->motors[motor].tacho_block_count;
}

int nxt_motor_get_rotation_count(nxt_t *nxt, int motor) {
  if (!NXT_VALID_MOTOR(motor)) {
    return NXT_FAIL;
  }

  nxt_motor_get_state(nxt, motor);
  return nxt->motors[motor].rotation_count;
}

int nxt_motor_get_pid(nxt_t *nxt, int motor, int *p, int *i, int *d) {
  int modid;
  char buf[3];

  if ((modid = nxt_mod_get_id(nxt,NXT_OUTPUT_MODFILE))!=-1) {
    if (nxt_mod_read(nxt, modid, buf, NXT_OUTPUT_PID(motor), 3)==NXT_FAIL) {
      return NXT_FAIL;
    }
    if (p!=NULL) {
      *p = buf[0];
    }
    if (i!=NULL) {
      *i = buf[1];
    }
    if (d!=NULL) {
      *d = buf[2];
    }
    return NXT_SUCC;
  }
  else {
    return NXT_FAIL;
  }
}

int nxt_motor_set_pid(nxt_t *nxt, int motor, int p, int i, int d) {
  int modid;
  char buf[3] = {p,i,d};

  if ((modid = nxt_mod_get_id(nxt,NXT_OUTPUT_MODFILE))!=-1) {
    return nxt_mod_write(nxt, modid, buf, NXT_OUTPUT_PID(motor), 3);
  }
  else {
    return NXT_FAIL;
  }
}

int nxt_motor_get_speed(nxt_t *nxt, int motor) {
  int modid;
  char speed = 0;

  if ((modid = nxt_mod_get_id(nxt,NXT_OUTPUT_MODFILE))!=-1) {
    nxt_mod_read(nxt, modid, &speed, NXT_OUTPUT_SPEED(motor), 1);
  }
  return speed;
}
