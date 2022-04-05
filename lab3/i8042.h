#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1

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

#endif
