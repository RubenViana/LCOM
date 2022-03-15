#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

#define LSB_MASK 0x1;
#define TWOLSB_MASK 0x3;
#define THREELSB_MASK 0x7;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  u_int8_t config = 0x00;
  
  config = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  sys_outb(TIMER_CTRL,config);

  if(timer==0){
    util_sys_inb(TIMER_0,st);
  }
  if(timer==1){
    util_sys_inb(TIMER_1,st);
  }
  if(timer==2){
    util_sys_inb(TIMER_2,st);
  }
  
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val value;

  switch (field){
    case tsf_all:value.byte=st;break;
    case tsf_initial: value.in_mode = (st >> 4) & TWOLSB_MASK;break;
    case tsf_mode:value.count_mode = (st >> 1) & THREELSB_MASK;break;
    case tsf_base:value.bcd = st & LSB_MASK;break;
  }

  timer_print_config(timer,field,value);

  return 1;
}
