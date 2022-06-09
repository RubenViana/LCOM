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
#include "assets/pokeball.xpm"
#include "assets/greatball.xpm"
#include "assets/masterball.xpm"
#include "assets/ultraball.xpm"
#include "assets/hover_play.xpm"
#include "assets/hover_exit.xpm"

#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern int counter;
extern uint8_t scancode;
extern int cnt;

extern bool reading_error;
extern int mouse_counter;
extern bool packet_read;
extern struct packet packet_struct;

extern unsigned h_res;	       
extern unsigned v_res;

extern bool M1_PRESSED;
extern bool MOUSE_HOVER_PLAY;
extern bool MOUSE_HOVER_EXIT;

int slot_pos = 0;
static int number_pokeballs = 12;

Sprite* mouse;
Sprite* play_background;
Sprite* menu_background;
Sprite* hover_play;
Sprite* hover_exit;
Sprite* player;
Sprite* goombas[16]; 
Sprite* pokeballs[12];

static int BLOCK_WIDTH = 72;
static int BLOCK_HEIGHT = 54;

bool UP = false;
bool DOWN = false;
bool LEFT = false;
bool RIGHT = false;




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

void clearKeys() {
    UP = false;
    DOWN = false;
    LEFT = false;
    RIGHT = false;
}

void initializeGame(){

    slot_pos = 0;

    for (int i = 0; i < number_pokeballs; i++){
        Sprite* pokeball;
        if (i < 3)
            pokeball = create_sprite(pokeball_xpm, i * 20, 0, 0, 0);
        else if (i < 6)
            pokeball = create_sprite(greatball_xpm, i * 20, 0, 0, 0);
        else if (i < 9)
            pokeball = create_sprite(ultraball_xpm, i * 20, 0, 0, 0);
        else
            pokeball = create_sprite(masterball_xpm, i * 20, 0, 0, 0);
        pokeballs[i] = pokeball;
    }
    
    for (int i = 0; i < 16; i++){
        Sprite* goomba = create_sprite(goomba_right_xpm,0,0,0,0);
        goombas[i] = goomba;
    }

    player->x = 550;
    player->y = 400;


    goombas[0]->xSpeed = (int)h_res/4 * 1;
    goombas[0]->ySpeed = -30;

    goombas[1]->xSpeed = (int)h_res/4 * 2;
    goombas[1]->ySpeed = -30;

    goombas[2]->xSpeed = (int)h_res/4 * 3;
    goombas[2]->ySpeed = -30;

    goombas[3]->xSpeed = 1130;
    goombas[3]->ySpeed = (int)v_res/4 * 1;

    goombas[4]->xSpeed = 1130;
    goombas[4]->ySpeed = (int)v_res/4 * 2;

    goombas[5]->xSpeed = 1130;
    goombas[5]->ySpeed = (int)v_res/4 * 3;

    goombas[6]->xSpeed = (int)h_res/4 * 1;
    goombas[6]->ySpeed = 834;

    goombas[7]->xSpeed = (int)h_res/4 * 2;
    goombas[7]->ySpeed = 834;

    goombas[8]->xSpeed = (int)h_res/4 * 3;
    goombas[8]->ySpeed = 834;

    goombas[9]->xSpeed = -30;
    goombas[9]->ySpeed = (int)v_res/4 * 1;

    goombas[10]->xSpeed = -30;
    goombas[10]->ySpeed = (int)v_res/4 * 2;

    goombas[11]->xSpeed = -30;
    goombas[11]->ySpeed = (int)v_res/4 * 3;

    goombas[12]->xSpeed = -20;
    goombas[12]->ySpeed = -20;

    goombas[13]->xSpeed = -20;
    goombas[13]->ySpeed = (int)v_res + 20;

    goombas[14]->xSpeed = (int)h_res + 20;
    goombas[14]->ySpeed = -20;

    goombas[15]->xSpeed = (int)h_res + 20;
    goombas[15]->ySpeed = (int)v_res + 20;

    clearKeys();

    for (int i = 0; i < 16; i++){
        goombas[i]->x = goombas[i]->xSpeed;
        goombas[i]->y = goombas[i]->ySpeed;
    }
}

void moveBullets() {
    for (int i = 0; i < number_pokeballs; i++){
        if (pokeballs[i]->x < BLOCK_WIDTH || pokeballs[i]->x+pokeballs[i]->width > (int)h_res-BLOCK_WIDTH || pokeballs[i]->y < BLOCK_HEIGHT || pokeballs[i]->y+pokeballs[i]->height > (int)v_res-BLOCK_HEIGHT){
            pokeballs[i]->x = i * 20;
            pokeballs[i]->y = 0;
            pokeballs[i]->xSpeed = 0;
            pokeballs[i]->ySpeed = 0;
        }
        pokeballs[i]->x += pokeballs[i]->xSpeed;
        pokeballs[i]->y += pokeballs[i]->ySpeed;
    }
}

void movePlayer() {
    if (UP && !DOWN) player->y -= player->ySpeed;
    if (DOWN && !UP) player->y += player->ySpeed;
    if (LEFT && !RIGHT) {
        player->x -= player->xSpeed;
        player = create_sprite(dooper_left_xpm, player->x, player->y, player->xSpeed, player->ySpeed);
    }
    if (RIGHT && !LEFT) {
        player->x += player->xSpeed;
        player = create_sprite(dooper_right_xpm, player->x, player->y, player->xSpeed, player->ySpeed);
    }
}

bool checkGoombaCollisions(int i){
    //collision between goombas
    for(int j = 0; j < 16;j++){
        if(i!=j){
            if(goombas[i]->x < goombas[j]->x + goombas[j]->width &&
            goombas[i]->x + goombas[i]->width > goombas[j]->x &&
            goombas[i]->y < goombas[j]->y + goombas[j]->height &&
            goombas[i]->y + goombas[i]->height > goombas[j]->y){
                return true;
            }
        }
    }

    //collisions with pokeballs
    for (int j = 0; j < number_pokeballs; j++){
        if(goombas[i]->x < pokeballs[j]->x + pokeballs[j]->width &&
            goombas[i]->x + goombas[i]->width > pokeballs[j]->x &&
            goombas[i]->y < pokeballs[j]->y + pokeballs[j]->height &&
            goombas[i]->y + goombas[i]->height > pokeballs[j]->y){
                goombas[i]->x = goombas[i]->xSpeed;
                goombas[i]->y = goombas[i]->ySpeed;
            }
    }

    return false;
}

void checkPlayerCollisions(Sprite *sp,state_g *gameState){

    //collisions with borders
    if(sp->x + sp->width > ((int)h_res - BLOCK_WIDTH)) {
        sp->x = ((int)h_res - BLOCK_WIDTH) - sp->width;
    }else if(sp->x < (0 + BLOCK_WIDTH)){
        sp->x = BLOCK_WIDTH;
    }else if(sp->y + sp->height > ((int)v_res - BLOCK_HEIGHT)){
        sp->y = ((int)v_res - BLOCK_HEIGHT) - sp->height;
    }
    else if(sp->y < (0 + BLOCK_HEIGHT)){
        sp->y = BLOCK_HEIGHT;
    }
    
    
    //collisions with goombas
    for (int i = 0; i < 16; i++){
        if(goombas[i]->x + 23 < sp->x + sp->width &&
            goombas[i]->x + goombas[i]->width - 23> sp->x &&
            goombas[i]->y + 23 < sp->y + sp->height &&
            goombas[i]->y + goombas[i]->height - 23> sp->y){
                    *gameState = MENU;
                    break;
                }
    }
    
}

void moveGoombas() {
    for (int i = 0; i < 16; i++){
        if (goombas[i]->x < player->x ) {
            goombas[i]->x ++;
            if(checkGoombaCollisions(i)){
                goombas[i]->x --;
            }
        }
        if (goombas[i]->x > player->x ) {
            goombas[i]->x --;
            if(checkGoombaCollisions(i)){
                goombas[i]->x ++;
            }
        }
        if (goombas[i]->y < player->y ) {
            goombas[i]->y ++;
            if(checkGoombaCollisions(i)){
                goombas[i]->y --;
            }
        }
        if (goombas[i]->y > player->y ) {
            goombas[i]->y --;
            if(checkGoombaCollisions(i)){
                goombas[i]->y ++;
            }
        }
    }
    
}

void updateScreen (state_g *gameState) {
    switch(*gameState){
        case MENU:
            mouse = create_sprite(mouse_xpm, mouse->x, mouse->y,0,0);       //maybe temp here
            draw_sprite_proj(*menu_background);

            //draw hover buttons , maybe temp
            if (MOUSE_HOVER_PLAY) draw_sprite_proj(*hover_play);
            else if (MOUSE_HOVER_EXIT) draw_sprite_proj(*hover_exit);

            draw_sprite_proj(*mouse);
            double_buffer();
            break;
        case PLAY:
            mouse = create_sprite(crosshair_xpm, mouse->x, mouse->y,0,0);       //maybe temp here
            draw_sprite_proj(*play_background);
            for (int i = 0; i < 16; i++){
                draw_sprite_proj(*goombas[i]);
            }
            for (int i = 0; i < number_pokeballs; i++){
                draw_sprite_proj(*pokeballs[i]);
            }
            draw_sprite_proj(*player);
            draw_sprite_proj(*mouse);
            double_buffer();

            movePlayer();
            moveBullets();
            moveGoombas();
            checkPlayerCollisions(player,gameState);

            break;
        default:
            break;
    }
}

void updateStateKbd (state_g *gameState){
    switch(*gameState){
        case MENU:
            if(scancode == ESCAPE_CODE){
                *gameState = GAME_OVER;
            }
            break;
        case PLAY:
            if(scancode == ESCAPE_CODE){
                *gameState = MENU;
            }

            //KEY PRESSED
            if(scancode == KEY_A_MAKECODE){
                LEFT = true;             
            }
            if(scancode == KEY_W_MAKECODE){
                UP = true;
            }
            if(scancode == KEY_S_MAKECODE){
                DOWN = true;
            }
            if(scancode == KEY_D_MAKECODE){
                RIGHT = true;
            }

            //KEY RELEASED
            if(scancode == KEY_A_BREAKCODE){
                LEFT = false;
            }
            if(scancode == KEY_W_BREAKCODE){
                UP = false;
            }
            if(scancode == KEY_S_BREAKCODE){
                DOWN = false;
            }
            if(scancode == KEY_D_BREAKCODE){
                RIGHT = false;
            }

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
        switch (*gameState){
            case MENU:
                if (M1_PRESSED){
                    if (MOUSE_HOVER_PLAY) {
                        *gameState = PLAY;
                        initializeGame();
                    }
                    else if (MOUSE_HOVER_EXIT) {
                        *gameState = GAME_OVER;
                    }
                    M1_PRESSED = false;
                }
                break;
            case PLAY:
                if(M1_PRESSED){
                    double dist_player_goomba = sqrt(((((mouse->x + mouse->width)/2) - (player->x + player->width)/2)*(((mouse->x + mouse->width)/2) - (player->x + player->width)/2)) + ((((mouse->y + mouse->height)/2) - (player->y + player->height)/2)*(((mouse->y + mouse->height)/2) - (player->y + player->height)/2)));
                    if (pokeballs[slot_pos%number_pokeballs]->y == 0) {
                        pokeballs[slot_pos%number_pokeballs]->x = player->x;
                        pokeballs[slot_pos%number_pokeballs]->y = player->y;
                        pokeballs[slot_pos%number_pokeballs]->xSpeed = ((((mouse->x + mouse->width)/2) - (player->x + player->width)/2) / dist_player_goomba) * 10;
                        pokeballs[slot_pos%number_pokeballs]->ySpeed = ((((mouse->y + mouse->height)/2) - (player->y + player->height)/2) / dist_player_goomba) * 10;
                    }
                    slot_pos++;
                    M1_PRESSED = false;
                }
                break;
            default: break;
        }
    }
}

int(proj_main_loop)(int argc, char *argv[]) {

    uint16_t mode = 0x14C;

    play_background = create_sprite(background_xpm,0,0,0,0);
    menu_background = create_sprite(menu_background_xpm,0,0,0,0);
    hover_play = create_sprite(hover_play_xpm,0,0,0,0);
    hover_exit = create_sprite(hover_exit_xpm,0,0,0,0);
    player = create_sprite(dooper_right_xpm,550,400,5,5);
    mouse = create_sprite(mouse_xpm,750,350,0,0);

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
                    updateScreen(&gameState);
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
