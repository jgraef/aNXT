/*
    hid.h
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

#ifndef _NXT_I2C_HID_H_
#define _NXT_I2C_HID_H_

#include <anxt/nxt.h>

#define NXT_HID_REG_MODIFIER 0x42
#define NXT_HID_REG_KEYDATA  0x43

#define NXT_HID_CMD_TRANSMIT      0x54
#define NXT_HID_CMD_SETASCIIMODE  0x41
#define NXT_HID_CMD_SETDIRECTMODE 0x44

#define NXT_HID_MODE_ASCII  NXT_HID_CMD_SETASCIIMODE
#define NXT_HID_MODE_DIRECT NXT_HID_CMD_SETDIRECTMODE

#define nxt_hid_get_version(nxt,port)  nxt_i2c_get_version(nxt,port,nxt_hid_i2c_addr)
#define nxt_hid_get_vendorid(nxt,port) nxt_i2c_get_vendorid(nxt,port,nxt_hid_i2c_addr)
#define nxt_hid_get_deviceid(nxt,port) nxt_i2c_get_deviceid(nxt,port,nxt_hid_i2c_addr)
#define nxt_hid_cmd(nxt,port,cmd)      nxt_i2c_cmd(nxt,port,nxt_hid_i2c_addr,cmd)

#define NXT_HID_MOD_NONE       0x00
#define NXT_HID_MOD_LEFTCTRL   0x01
#define NXT_HID_MOD_LEFTSHIFT  0x02
#define NXT_HID_MOD_LEFTALT    0x04
#define NXT_HID_MOD_LEFTGUI    0x08
#define NXT_HID_MOD_RIGHTCTRL  0x10
#define NXT_HID_MOD_RIGHTSHIFT 0x20
#define NXT_HID_MOD_RIGHTALT   0x40
#define NXT_HID_MOD_RIGHTGUI   0x80

#define NXT_HID_KEY_NOEVENT             0x00
#define NXT_HID_KEY_ERROVERUN           0x01
#define NXT_HID_KEY_POSTFAIL            0x02
#define NXT_HID_KEY_ERRUNDEF            0x03
#define NXT_HID_KEY_A                   0x04
#define NXT_HID_KEY_B                   0x05
#define NXT_HID_KEY_C                   0x06
#define NXT_HID_KEY_D                   0x07
#define NXT_HID_KEY_E                   0x08
#define NXT_HID_KEY_F                   0x09
#define NXT_HID_KEY_G                   0x0A
#define NXT_HID_KEY_H                   0x0B
#define NXT_HID_KEY_I                   0x0C
#define NXT_HID_KEY_J                   0x0D
#define NXT_HID_KEY_K                   0x0E
#define NXT_HID_KEY_L                   0x0F
#define NXT_HID_KEY_M                   0x10
#define NXT_HID_KEY_N                   0x11
#define NXT_HID_KEY_O                   0x12
#define NXT_HID_KEY_P                   0x13
#define NXT_HID_KEY_Q                   0x14
#define NXT_HID_KEY_R                   0x15
#define NXT_HID_KEY_S                   0x16
#define NXT_HID_KEY_T                   0x17
#define NXT_HID_KEY_U                   0x18
#define NXT_HID_KEY_V                   0x19
#define NXT_HID_KEY_W                   0x1A
#define NXT_HID_KEY_X                   0x1B
#define NXT_HID_KEY_Y                   0x1C
#define NXT_HID_KEY_Z                   0x1D
#define NXT_HID_KEY_1                   0x1E
#define NXT_HID_KEY_EXCLAIM             0x1E
#define NXT_HID_KEY_2                   0x1F
#define NXT_HID_KEY_AT                  0x1F
#define NXT_HID_KEY_3                   0x20
#define NXT_HID_KEY_HASH                0x20
#define NXT_HID_KEY_4                   0x21
#define NXT_HID_KEY_DOLLAR              0x21
#define NXT_HID_KEY_5                   0x22
#define NXT_HID_KEY_PERCENT             0x22
#define NXT_HID_KEY_6                   0x23
#define NXT_HID_KEY_CARET               0x23
#define NXT_HID_KEY_7                   0x24
#define NXT_HID_KEY_AMPERSAND           0x24
#define NXT_HID_KEY_8                   0x25
#define NXT_HID_KEY_ASTERISK            0x25
#define NXT_HID_KEY_9                   0x26
#define NXT_HID_KEY_LEFTPAREN           0x26
#define NXT_HID_KEY_0                   0x27
#define NXT_HID_KEY_RIGHTPAREN          0x27
#define NXT_HID_KEY_RETURN              0x28
#define NXT_HID_KEY_ESCAPE              0x29
#define NXT_HID_KEY_BACKSPACE           0x2A
#define NXT_HID_KEY_TAB                 0x2B
#define NXT_HID_KEY_SPACE               0x2C
#define NXT_HID_KEY_MINUS               0x2D
#define NXT_HID_KEY_UNDERBAR            0x2D
#define NXT_HID_KEY_EQUAL               0x2E
#define NXT_HID_KEY_PLUS                0x2E
#define NXT_HID_KEY_LEFTBRACKET         0x2F
#define NXT_HID_KEY_LEFTBRACE           0x2F
#define NXT_HID_KEY_RIGHTBRACKET        0x30
#define NXT_HID_KEY_RIGHTBRACE          0x30
#define NXT_HID_KEY_BACKSLASH           0x31
#define NXT_HID_KEY_PIPE                0x31
#define NXT_HID_KEY_EUROPE1             0x32
#define NXT_HID_KEY_SEMICOLON           0x33
#define NXT_HID_KEY_COLON               0x33
#define NXT_HID_KEY_QUOTE               0x34
#define NXT_HID_KEY_DOUBLEQUOTE         0x34
#define NXT_HID_KEY_APOSTROPHE          0x35
#define NXT_HID_KEY_TILDE               0x35
#define NXT_HID_KEY_COMMA               0x36
#define NXT_HID_KEY_LESS                0x36
#define NXT_HID_KEY_PERIOD              0x37
#define NXT_HID_KEY_GREATER             0x37
#define NXT_HID_KEY_SLASH               0x38
#define NXT_HID_KEY_QUESTION            0x38
#define NXT_HID_KEY_CAPSLOCK            0x39
#define NXT_HID_KEY_F1                  0x3A
#define NXT_HID_KEY_F2                  0x3B
#define NXT_HID_KEY_F3                  0x3C
#define NXT_HID_KEY_F4                  0x3D
#define NXT_HID_KEY_F5                  0x3E
#define NXT_HID_KEY_F6                  0x3F
#define NXT_HID_KEY_F7                  0x40
#define NXT_HID_KEY_F8                  0x41
#define NXT_HID_KEY_F9                  0x42
#define NXT_HID_KEY_F10                 0x43
#define NXT_HID_KEY_F11                 0x44
#define NXT_HID_KEY_F12                 0x45
#define NXT_HID_KEY_PRINTSCREEN         0x46
#define NXT_HID_KEY_SCROLLLOCK          0x47
#define NXT_HID_KEY_BREAK               0x48
#define NXT_HID_KEY_PAUSE               0x48
#define NXT_HID_KEY_INSERT              0x49
#define NXT_HID_KEY_HOME                0x4A
#define NXT_HID_KEY_PAGEUP              0x4B
#define NXT_HID_KEY_DELETE              0x4C
#define NXT_HID_KEY_END                 0x4D
#define NXT_HID_KEY_PAGEDOWN            0x4E
#define NXT_HID_KEY_RIGHT               0x4F
#define NXT_HID_KEY_LEFT                0x50
#define NXT_HID_KEY_DOWN                0x51
#define NXT_HID_KEY_UP                  0x52
#define NXT_HID_KEY_NUMLOCK             0x53
#define NXT_HID_KEY_KEYPAD_SLASH        0x54
#define NXT_HID_KEY_KEYPAD_ASTERISK     0x55
#define NXT_HID_KEY_KEYPAD_MINUS        0x56
#define NXT_HID_KEY_KEYPAD_PLUS         0x57
#define NXT_HID_KEY_KEYPAD_ENTER        0x58
#define NXT_HID_KEY_KEYPAD_1            0x59
#define NXT_HID_KEY_KEYPAD_END          0x59
#define NXT_HID_KEY_KEYPAD_2            0x5A
#define NXT_HID_KEY_KEYPAD_DOWN         0x5A
#define NXT_HID_KEY_KEYPAD_3            0x5B
#define NXT_HID_KEY_KEYPAD_PAGEDOWN     0x5B
#define NXT_HID_KEY_KEYPAD_4            0x5C
#define NXT_HID_KEY_KEYPAD_LEFT         0x5C
#define NXT_HID_KEY_KEYPAD_5            0x5D
#define NXT_HID_KEY_KEYPAD_6            0x5E
#define NXT_HID_KEY_KEYPAD_RIGHT        0x5E
#define NXT_HID_KEY_KEYPAD_7            0x5F
#define NXT_HID_KEY_KEYPAD_HOME         0x5F
#define NXT_HID_KEY_KEYPAD_8            0x60
#define NXT_HID_KEY_KEYPAD_UP           0x60
#define NXT_HID_KEY_KEYPAD_9            0x61
#define NXT_HID_KEY_KEYPAD_PAGEUP       0x61
#define NXT_HID_KEY_KEYPAD_0            0x62
#define NXT_HID_KEY_KEYPAD_INSERT       0x62
#define NXT_HID_KEY_KEYPAD_PERIOD       0x63
#define NXT_HID_KEY_KEYPAD_DELETE       0x63
#define NXT_HID_KEY_EUROPE2             0x64
#define NXT_HID_KEY_APP                 0x65
#define NXT_HID_KEY_KEYBOARD_POWER      0x66
#define NXT_HID_KEY_KEYPAD_EQUAL        0x67
#define NXT_HID_KEY_F13                 0x68
#define NXT_HID_KEY_F14                 0x68
#define NXT_HID_KEY_F15                 0x6A
#define NXT_HID_KEY_F16                 0x6B
#define NXT_HID_KEY_F17                 0x6C
#define NXT_HID_KEY_F18                 0x6D
#define NXT_HID_KEY_F19                 0x6E
#define NXT_HID_KEY_F20                 0x6F
#define NXT_HID_KEY_F21                 0x70
#define NXT_HID_KEY_F22                 0x71
#define NXT_HID_KEY_F23                 0x72
#define NXT_HID_KEY_F24                 0x73
#define NXT_HID_KEY_KEYBOARD_EXEC       0x74
#define NXT_HID_KEY_KEYBOARD_HELP       0x75
#define NXT_HID_KEY_KEYBOARD_MENU       0x76
#define NXT_HID_KEY_KEYBOARD_SELECT     0x77
#define NXT_HID_KEY_KEYBOARD_STOP       0x78
#define NXT_HID_KEY_KEYBOARD_AGAIN      0x79
#define NXT_HID_KEY_KEYBOARD_UNDO       0x7A
#define NXT_HID_KEY_KEYBOARD_CUT        0x7B
#define NXT_HID_KEY_KEYBOARD_COPY       0x7C
#define NXT_HID_KEY_KEYBOARD_PASTE      0x7D
#define NXT_HID_KEY_KEYBOARD_FIND       0x7E
#define NXT_HID_KEY_KEYBOARD_MUTE       0x7F
#define NXT_HID_KEY_KEYBOARD_VOLUP      0x80
#define NXT_HID_KEY_KEYBOARD_VOLDOWN    0x81
#define NXT_HID_KEY_KEYBOARD_CAPSLOCK   0x82
#define NXT_HID_KEY_KEYBOARD_NUMLOCK    0x83
#define NXT_HID_KEY_KEYBOARD_SCROLLLOCK 0x84
#define NXT_HID_KEY_KEYPAD_COMMA        0x85
#define NXT_HID_KEY_KEYBOARD_EQUAL      0x86
#define NXT_HID_KEY_KEYBOARD_INTL1      0x87
#define NXT_HID_KEY_KEYBOARD_INTL2      0x88
#define NXT_HID_KEY_KEYBOARD_INTL3      0x89
#define NXT_HID_KEY_KEYBOARD_INTL4      0x8A
#define NXT_HID_KEY_KEYBOARD_INTL5      0x8B
#define NXT_HID_KEY_KEYBOARD_INTL6      0x8C
#define NXT_HID_KEY_KEYBOARD_INTL7      0x8D
#define NXT_HID_KEY_KEYBOARD_INTL8      0x8E
#define NXT_HID_KEY_KEYBOARD_INTL9      0x8F
#define NXT_HID_KEY_KEYBOARD_LANG1      0x90
#define NXT_HID_KEY_KEYBOARD_LANG2      0x91
#define NXT_HID_KEY_KEYBOARD_LANG3      0x92
#define NXT_HID_KEY_KEYBOARD_LANG4      0x93
#define NXT_HID_KEY_KEYBOARD_LANG5      0x94
#define NXT_HID_KEY_KEYBOARD_LANG6      0x95
#define NXT_HID_KEY_KEYBOARD_LANG7      0x96
#define NXT_HID_KEY_KEYBOARD_LANG8      0x97
#define NXT_HID_KEY_KEYBOARD_LANG9      0x98
#define NXT_HID_KEY_KEYBOARD_ALT_EREASE 0x99
#define NXT_HID_KEY_KEYBOARD_SYSREQ     0x9A
#define NXT_HID_KEY_KEYBOARD_ATTENTION  0x9A
#define NXT_HID_KEY_KEYBOARD_CANCEL     0x9B
#define NXT_HID_KEY_KEYBOARD_CLEAR      0x9C
#define NXT_HID_KEY_KEYBOARD_PRIOR      0x9D
#define NXT_HID_KEY_KEYBOARD_RETURN     0x9E
#define NXT_HID_KEY_KEYBOARD_SEPARATOR  0x9F
#define NXT_HID_KEY_KEYBOARD_OUT        0xA0
#define NXT_HID_KEY_KEYBOARD_OPER       0xA1
#define NXT_HID_KEY_KEYBOARD_CLEAR2     0xA2
#define NXT_HID_KEY_KEYBOARD_AGAIN2     0xA2
#define NXT_HID_KEY_KEYBOARD_CRSEL      0xA3
#define NXT_HID_KEY_KEYBOARD_PROPS      0xA3
#define NXT_HID_KEY_KEYBOARD_EXSEL      0xA4
#define NXT_HID_KEY_LEFTCONTROL         0xE0
#define NXT_HID_KEY_LEFTSHIFT           0xE1
#define NXT_HID_KEY_LEFTALT             0xE2
#define NXT_HID_KEY_LEFTGUI             0xE3
#define NXT_HID_KEY_RIGHTCONTROL        0xE4
#define NXT_HID_KEY_RIGHTSHIFT          0xE5
#define NXT_HID_KEY_RIGHTALT            0xE6
#define NXT_HID_KEY_RIGHTGUI            0xE7
#define NXT_HID_KEY_SYSPOWER            0x81
#define NXT_HID_KEY_SYSSLEEP            0x82
#define NXT_HID_KEY_SYSWAKE             0x83

int nxt_hid_i2c_addr;

int nxt_hid_set_modifier(nxt_t *nxt, int port, int modifier);
int nxt_hid_set_key(nxt_t *nxt, int port, int key);

static __inline__ int nxt_hid_transmit(nxt_t *nxt, int port) {
  return nxt_i2c_cmd(nxt, port, nxt_hid_i2c_addr, NXT_HID_CMD_TRANSMIT);
}

static __inline__ int nxt_hid_set_mode(nxt_t *nxt, int port, int mode) {
  return nxt_i2c_cmd(nxt, port, nxt_hid_i2c_addr, mode);
}

#endif /* _NXT_I2C_HID_H_ */
