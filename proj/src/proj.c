#include <lcom/lcf.h>

#include "keyboard.h"
#include "videocard.h"

#include "mouse.h"

#include "assets/background.xpm"
#include "assets/menu_background.xpm"
#include "assets/dooper_right.xpm"
#include "assets/dooper_left.xpm"
#include "assets/goomba_right.xpm"
#include "assets/mouse.xpm"
#include "assets/crosshair.xpm"

#include <stdint.h>
#include <stdio.h>

extern int counter;
extern uint8_t scancode;
extern int cnt;

extern bool reading_error;
extern int mouse_counter;
extern bool packet_read;
extern struct packet packet_struct;

extern unsigned h_res;	       
extern unsigned v_res;

Sprite* mouse;
Sprite* play_background;
Sprite* menu_background;
Sprite* player;
Sprite* goombas[10]; 

static int BLOCK_WIDTH = 72;
static int BLOCK_HEIGHT = 54;

typedef enum { MENU, PLAY, GAME_OVER} state_g;

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

void updateScreen (state_g gameState) {
    switch(gameState){
        case MENU:
            draw_sprite_proj(*menu_background);
            draw_sprite_proj(*mouse);
            double_buffer();
            break;
        case PLAY:
            draw_sprite_proj(*play_background);
            for (int i = 0; i < 10; i++){
                if (goombas[i]->x > 0 && goombas[i]->y > 0)
                    draw_sprite_proj(*goombas[i]);
            }
            draw_sprite_proj(*player);
            draw_sprite_proj(*mouse);
            double_buffer();
            break;
        default:
            break;
    }
}

void checkCollisions(Sprite *sp){

    //collisions with all
    if(sp->x + sp->width > ((int)h_res - BLOCK_WIDTH)) {
        sp->x = ((int)h_res - BLOCK_WIDTH) - sp->width;
    }else if(sp->x < (0 + BLOCK_WIDTH)){
        sp->x = BLOCK_WIDTH;
    }else if(sp->y + sp->height > ((int)v_res + BLOCK_HEIGHT)){
        sp->y = ((int)v_res + BLOCK_HEIGHT) - sp->height;
    }
    else if(sp->y < (0 + BLOCK_HEIGHT)){
        sp->y = BLOCK_HEIGHT;
    }
    
    /*
    //collisions with goombas
    for (int i = 0; i < 10; i++){
        if(sp->x + sp->width > (goombas[i]->x - goombas[i]->width)) {
            sp->x = (goombas[i]->x - goombas[i]->width) - sp->width;
        }else if(sp->x < (goombas[i]->x)){
            sp->x = goombas[i]->x + goombas[i]->width;
        }else if(sp->y + sp->height > (goombas[i]->y + goombas[i]->height)){
            sp->y = (goombas[i]->y + goombas[i]->height) - sp->height;
        }
        else if(sp->y < (goombas[i]->y)){
            sp->y = goombas[i]->y;
        }
    }
    */
}

void moveGoombas() {
    for (int i = 0; i < 10; i++){
        if (goombas[i]->x > 0 && goombas[i]->y > 0) {
            if (goombas[i]->x < player->x ) {
                goombas[i]->x ++;
            }
            if (goombas[i]->x > player->x ) {
                goombas[i]->x --;
            }
            if (goombas[i]->y < player->y ) {
                goombas[i]->y ++;
            }
            if (goombas[i]->y > player->y ) {
                goombas[i]->y --;
            }
        }
    }
}

void updateStateKbd (state_g *gameState){
    switch(*gameState){
        case MENU:
            if(scancode == ESCAPE_CODE){
                *gameState = GAME_OVER;
            }
            else if(scancode == SPACEBAR_CODE){
                *gameState = PLAY;
                mouse = create_sprite(crosshair_xpm, mouse->x, mouse->y);
            }
            break;
        case PLAY:
            if(scancode == ESCAPE_CODE){
                *gameState = MENU;
                mouse = create_sprite(mouse_xpm, mouse->x, mouse->y);
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

            checkCollisions(player);
            

            break;
        default:
            break;
    }
}

void updateStateMouse (state_g *gameState){
    if (reading_error == true && mouse_counter != 2) {
        return;
    }
    else if (reading_error == true && mouse_counter == 2) {
        reading_error = false;
    }
    else{
        organize_packets();
    }
}

int(proj_main_loop)(int argc, char *argv[]) {

    uint16_t mode = 0x14C;

    play_background = create_sprite(background_xpm,0,0);
    menu_background = create_sprite(menu_background_xpm,0,0);
    player = create_sprite(dooper_right_xpm,100,200);
    mouse = create_sprite(mouse_xpm,500,500);
    
    for (int i = 0; i < 10; i++){
        Sprite* goomba = create_sprite(goomba_right_xpm, -100, -100);
        goombas[i] = goomba;
    }

    goombas[0]->x = 800;
    goombas[0]->y = 200;

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
                    moveGoombas();
                    updateScreen(gameState);
                }
                if (msg.m_notify.interrupts & kbd_int_bit) {
                    kbc_ih();
                    updateStateKbd(&gameState);
                }
                if (msg.m_notify.interrupts & mouse_int_bit) {
                    mouse_ih();
                    updateStateMouse(&gameState);
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
