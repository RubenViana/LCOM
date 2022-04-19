#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <lcom/lcf.h>
#include "i8042.h"
#include <stdint.h>


int (kbc_subscribe_int)(uint8_t * bit_no);

int (kbc_unsubscribe_int)();

void (kbc_ih)();

int (kbc_print_scancode)();

int (readStatus)();

#endif
