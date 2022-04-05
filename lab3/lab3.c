
#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

extern uint8_t scancode;
extern int cnt;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;
  u_int8_t bit_no = 1;
  u_int32_t irq_set = BIT(bit_no);
  message msg;
  kbc_subscribe_int(&bit_no);
  while (scancode != ESCAPE_CODE) { 
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            kbc_print_scancode();
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();
  kbd_print_no_sysinb(cnt);
  return 0;
}

int(kbd_test_poll)() {

  while (scancode != ESCAPE_CODE) { 
    if (readStatus() != 0) return 1;
    kbc_print_scancode();
  }

  if (sys_outb(IN_BUF_COMMANDS, READ_COMMAND) != 0) return 1;
  readStatus();
  if (sys_outb(IN_BUF_COMMANDS, WRITE_COMMAND) != 0) return 1;
  if (sys_outb(IN_BUF_ARGS, scancode | BIT(0)) != 0) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  u_int8_t bit_no1 = 1;
  u_int8_t bit_no0 = 0;
  u_int32_t timer0_int_bit = BIT(bit_no0);
  u_int32_t kbd_int_bit = BIT(bit_no1);
  message msg;
  kbc_subscribe_int(&bit_no1);
  timer_subscribe_int(&bit_no0);
  int frequency = 60;
  //timer_set_frequency(0,frequency);

  while (scancode != ESCAPE_CODE) { 
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             
          if (msg.m_notify.interrupts & timer0_int_bit) {
              timer_int_handler();
              if(counter  == frequency * n){
                scancode = ESCAPE_CODE;
              }
          }break;
          if (msg.m_notify.interrupts & kbd_int_bit) { 
            kbc_ih();
            kbc_print_scancode();
            counter = 0;
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();
  timer_unsubscribe_int();
  kbd_print_no_sysinb(cnt);
  return 0;
}

