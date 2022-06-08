#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1

#define MOUSE_IRQ 12

#define DELAY_US    20000

#define OUT_BUFF_BIT BIT(0)

#define IN_BUFF_BIT BIT(1)

#define KBC_PAR_ERROR BIT(7)

#define KBC_TO_ERROR BIT(6)

#define AUX BIT(5)

#define STATUS_REGISTER 0x64 

#define IN_BUF_COMMANDS 0x64 

#define OUT_BUF 0x60 

#define IN_BUF_ARGS 0x60

#define READ_COMMAND 0x20

#define WRITE_COMMAND 0x60

#define ESCAPE_CODE 0x81 

#define SPACEBAR_CODE 0x39

#define KEY_A_MAKECODE 0x1E

#define KEY_W_MAKECODE 0x11

#define KEY_S_MAKECODE 0x1F

#define KEY_D_MAKECODE 0x20

#define KEY_A_BREAKCODE 0x9E

#define KEY_W_BREAKCODE 0x91

#define KEY_S_BREAKCODE 0x9F

#define KEY_D_BREAKCODE 0xa0

#define COMMAND_TO_MOUSE 0xD4

#define NACK 0xFE

#define ERROR 0xFC

#define ACK 0xFA

#define ENABLE_MOUSE_DATA 0xF4

#define DISABLE_MOUSE_DATA 0xF5

#define SET_STREAM_MODE 0xEA

#define SET_REMOTE_MODE 0xF0

#define READ_DATA_MOUSE 0xEB

#define RESET_MOUSE 0xFF

#endif
