#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

#define LSB_MASK 0x1;
#define TWOLSB_MASK 0x3;
#define THREELSB_MASK 0x7;
#define FOURLSB_MASK 0XF;

int hook_id_global = 0;

int counter = 0;

/**
 * Mudar a frequência de um timer
 * @param timer Timer
 * @param freq Frequência
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t control;
  timer_get_conf(timer,&control);
  control &= FOURLSB_MASK;
  control |= BIT(4) | BIT(5);
  switch(timer){
    case 0: break;
    case 1: control |= BIT(6);break;
    case 2: control |= BIT(7);break;
  }
  sys_outb(TIMER_CTRL,control);

  uint16_t div = TIMER_FREQ / freq;

  uint8_t LSB;
  util_get_LSB(div,&LSB);

  uint8_t MSB;
  util_get_MSB(div,&MSB);

  if(timer==0){
    sys_outb(TIMER_0,LSB);
    sys_outb(TIMER_0,MSB);
  }
  if(timer==1){
    sys_outb(TIMER_1,LSB);
    sys_outb(TIMER_1,MSB);
  }
  if(timer==2){
    sys_outb(TIMER_2,LSB);
    sys_outb(TIMER_2,MSB);
  }

  return 0;
}

/**
 * Subscrever as interrupções
 * @param bit_no hook_id
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (timer_subscribe_int)(uint8_t *bit_no) {
  int bit_no_int = *bit_no;
  if(bit_no==NULL){
    return 1;
  }
  sys_irqsetpolicy(CLOCK_IRQ,IRQ_REENABLE,&bit_no_int);
  hook_id_global = bit_no_int;

  return 0;
}

/**
 * Cancelar a subscrição
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (timer_unsubscribe_int)() {

  sys_irqrmpolicy(&hook_id_global);
  return 0;

}

/**
 * Interrupt handler - aumenta um contador
 */
void (timer_int_handler)() {
    ++counter;
}

/**
 * Obtém a configuração de um timer
 * @param timer Timer
 * @param st Estrutura onde guardar a configuração
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if(st==NULL) return 1;

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
  
  return 0;
}

/**
 * Mostrar a configuração de um timer
 * @param timer Timer
 * @param st Configuração
 * @param field Campo a mostrar
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val value;


  switch (field){
    case tsf_all:value.byte=st;break;
    case tsf_initial: value.in_mode = (st >> 4) & TWOLSB_MASK;break;
    case tsf_mode:{
      uint8_t res = (st >> 1) & THREELSB_MASK;
      if(res == 6 || res==7)
        res = res & TWOLSB_MASK;
      value.count_mode = res;
      }break;
    case tsf_base:value.bcd = st & LSB_MASK;break;
  }

  timer_print_config(timer,field,value);

  return 0;
}
