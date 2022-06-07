#include <lcom/lcf.h>

#include "keyboard.h"
#include "videocard.h"

#include "mouse.h"

#include "assets/background.xpm"
#include "assets/menu_background.xpm"
#include "assets/dooper_right.xpm"
#include "assets/dooper_left.xpm"


#include <stdint.h>
#include <stdio.h>

extern int counter;
extern uint8_t scancode;
extern int cnt;

extern bool reading_error;
extern int mouse_counter;
extern bool packet_read;
extern struct packet packet_struct;

Sprite* mouse;

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


    uint16_t mode = 0x14C;

    Sprite* play_background = create_sprite(background_xpm,0,0);
    Sprite* menu_background = create_sprite(menu_background_xpm,0,0);
    Sprite* player = create_sprite(dooper_right_xpm,100,200);
    mouse = create_sprite(ubuntu_xpm,500,500);
    
     
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

    if (enable_mouse_data() != 0)
        return 1;

    uint8_t bit_no_mouse = 2;
    uint32_t mouse_int_bit = BIT(bit_no_mouse);
    mouse_subscribe(&bit_no_mouse);

    int frequency = 30;

    timer_set_frequency(0, frequency);

    state_g gameState = MENU;

    while (gameState != GAME_OVER) {
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
                    switch(gameState){
                        case MENU:
                            draw_sprite_proj(*menu_background);
                            double_buffer();
                            break;
                        case PLAY:
                            draw_sprite_proj(*play_background);
                            draw_sprite_proj(*player);
                            draw_sprite_proj(*mouse);
                            double_buffer();
                            break;
                        default:
                            break;
                    }
                }
                if (msg.m_notify.interrupts & kbd_int_bit) {
                    kbc_ih();
                    switch(gameState){
                        case MENU:
                            if(scancode == ESCAPE_CODE){
                                gameState = GAME_OVER;
                            }
                            else if(scancode == SPACEBAR_CODE){
                                gameState = PLAY;
                            }
                            break;
                        case PLAY:
                            if(scancode == ESCAPE_CODE){
                                gameState = MENU;
                            }
                            else if(scancode == KEY_A_CODE){
                                player->x -= 10;
                                player = create_sprite(dooper_left_xpm, player->x, player->y);
                            }
                            else if(scancode == KEY_W_CODE){
                                player->y -= 10;
                            }
                            else if(scancode == KEY_S_CODE){
                                player->y += 10;
                            }
                            else if(scancode == KEY_D_CODE){
                                player->x += 10;
                                player = create_sprite(dooper_right_xpm, player->x, player->y);
                            }
                            break;
                        default:
                            break;
                    }
                }
                if (msg.m_notify.interrupts & mouse_int_bit) {
                    mouse_ih();
                    if (reading_error == true && mouse_counter != 2) {
                        continue;
                    }
                    else if (reading_error == true && mouse_counter == 2) {
                        reading_error = false;
                    }
                    else{
                        organize_packets();
                    }
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

    if (disable_mouse_data() != 0) return 1;

    if (vg_exit() != 0) {
        printf("\t vg_exit(): error ");
        return 1;
    }

    kbc_unsubscribe_int();
    timer_unsubscribe_int();
    mouse_unsubscribe();

    return 0;
}
