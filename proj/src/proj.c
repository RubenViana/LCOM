#include <lcom/lcf.h>

#include "keyboard.h"
#include "videocard.h"

#include <stdint.h>
#include <stdio.h>

extern int counter;
extern uint8_t scancode;
extern int cnt;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

typedef enum { MENU, PLAY, GAME_OVER} state_g;

int(proj_main_loop)(int argc, char *argv[]) {

    uint16_t mode = 0x14c;
    uint16_t xPos = 0;
    uint16_t yPos = 0;
    uint16_t width = 48;
    uint16_t height = 54;
    uint32_t color = 0xffffffff;

    if (vg_init(mode) == NULL) {
    printf("\t vg_init(): error ");
    return 1;
    }

    int ipc_status;
    u_int8_t bit_no1 = 1;
    u_int8_t bit_no0 = 0;
    u_int32_t timer0_int_bit = BIT(bit_no0);
    u_int32_t kbd_int_bit = BIT(bit_no1);
    message msg;
    kbc_subscribe_int(&bit_no1);
    timer_subscribe_int(&bit_no0);
    int frequency = 60;

    timer_set_frequency(0, frequency);

    state_g gameState = MENU;

    while (gameState != GAME_OVER){
        
    }

    while (scancode != ESCAPE_CODE) {
        int r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:                                    /* hardware interrupt notification */
                if (msg.m_notify.interrupts & timer0_int_bit) { /* subscribed interrupt */
                    timer_int_handler();
                    vg_draw_rectangle(xPos, yPos, width, height, color);
                    double_buffer();
                }

                if (msg.m_notify.interrupts & kbd_int_bit) {
                    kbc_ih();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */
            }
        }
        else { /* received a standard message, not a notification */
                /* no standard messages expected: do nothing */
        }
    }

    if (vg_exit() != 0) {
        printf("\t vg_exit(): error ");
        return 1;
    }

    kbc_unsubscribe_int();
    timer_unsubscribe_int();

    return 0;
}