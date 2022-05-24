#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>

#define MIN_SLOPE 1.0

typedef enum { INIT, DRAW_UP, DRAW_DOWN,
DETECTED } state_t;
typedef enum { RBDOWN, RBUP, LBDOWN, LBUP, MBDOWN,
MBUP, MOVE } event_type_t;

typedef struct {
event_type_t type;
uint8_t moveX;
uint8_t moveY;
bool lbdown, rbdown, mbdown;
} mouse_event_t;

int(mouse_subscribe)(uint8_t * bit_no);
int(mouse_unsubscribe)();
void(mouse_ih)();
void (organize_packets)();
int(write_argument_to_mouse)(uint8_t argument);
int(enable_mouse_data)();
int(disable_mouse_data)();
void (mouse_check_pattern)(mouse_event_t evt,uint8_t x_len,uint8_t tolerance);


#endif
