
#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "rtc.h"

extern uint8_t scancode;
extern int cnt;
extern int counter;
extern struct Date date;

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

  rtc_enable_update_alarm();
  uint8_t bit_no_rtc = 3;
  uint32_t irq_set_rtc = BIT(bit_no_rtc);
  rtc_subscribe();

  rtc_get_date();
  sys_outb(RTC_ADDR_REG, RTC_SECOND_ALARM_REG);
  sys_outb(RTC_DATA_REG, date.sec );

  sys_outb(RTC_ADDR_REG, RTC_MINUTE_ALARM_REG);
  sys_outb(RTC_DATA_REG, date.min + 0x1);

  sys_outb(RTC_ADDR_REG, RTC_HOUR_ALARM_REG);
  sys_outb(RTC_DATA_REG, date.hour);

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
            rtc_get_date();
            printf("Year: %02X ",date.year);
            printf("Month: %02X ",date.month);
            printf("Day: %02X ",date.day);
            printf("Hours: %02X ",date.hour);
            printf("Minutes: %02X ",date.min);
            printf("Seconds: %02X \n",date.sec);
          }
          if (msg.m_notify.interrupts & irq_set_rtc) {
             rtc_ih();
           }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();

  rtc_unsubscribe();

  rtc_disable_update_alarm();

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
  kbd_print_no_sysinb(cnt);
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
          }
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

