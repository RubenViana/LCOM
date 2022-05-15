#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>

int(mouse_subscribe)(uint8_t * bit_no);
int(mouse_unsubscribe)();
void(mouse_ih)();
void (organize_packets)();
int(write_argument_to_mouse)(uint8_t argument);
int(enable_mouse_data)();
int(disable_mouse_data)();
//void(read_from_mouse)();


#endif
