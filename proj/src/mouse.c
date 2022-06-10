#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"


int hook_id_mouse;
uint8_t packet;
bool synced = false;
struct packet packet_struct;
int mouse_counter = 0;
bool reading_error = false;
extern Sprite* mouse;
extern unsigned h_res;	       
extern unsigned v_res;
bool M1_PRESSED;
bool MOUSE_HOVER_PLAY;
bool MOUSE_HOVER_EXIT;
bool MOUSE_HOVER_PLAYAGAIN;
bool MOUSE_HOVER_GAMEOVER_EXIT;

int(mouse_subscribe)(uint8_t * bit_no){
    int bit_no_int = *bit_no;
    if(bit_no == NULL){
        return 1;
    }
    if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&bit_no_int)!=0){
        return 1;
    }
    hook_id_mouse = bit_no_int;
    return 0;
    
    return 0;
}
int(mouse_unsubscribe)(){
    if (sys_irqrmpolicy(&hook_id_mouse) != 0){
        return 1;
    }
   return 0;
}

void(mouse_ih)(){
    uint8_t stat;

    for(int i = 0; i<2;i++){
        if (util_sys_inb(STATUS_REGISTER, &stat) != 0) { 
            printf("Error reading stat\n");
            break;
        }

        if ((stat & OUT_BUFF_BIT) && (stat & AUX)){

            if (util_sys_inb(OUT_BUF, &packet) != 0) {
                printf("Error reading packet\n");
                break;
            }
            if ((stat & (KBC_PAR_ERROR | KBC_TO_ERROR)) == 0) {
                if (reading_error == true){
                    mouse_counter ++;
                }
                return;
            }
            else{
                printf("Parity error or timeout\n");
                break;
            }
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
    reading_error = true;
    mouse_counter ++;
}

void (update_mouse_pos)() {
    if (packet_struct.x_ov || packet_struct.y_ov) return;

    mouse->x += packet_struct.delta_x;
    mouse->y -= packet_struct.delta_y;

    M1_PRESSED = packet_struct.lb;
    MOUSE_HOVER_PLAY = mouse->x > 504 && mouse->x < 640 && mouse->y > 429 && mouse->y < 465;
    MOUSE_HOVER_EXIT = mouse->x > 504 && mouse->x < 640 && mouse->y > 482 && mouse->y < 516;
    MOUSE_HOVER_PLAYAGAIN = mouse->x > 389 && mouse->x < 603 && mouse->y > 515 && mouse->y < 535;
    MOUSE_HOVER_GAMEOVER_EXIT = mouse->x > 675 && mouse->x < 757 && mouse->y > 515 && mouse->y < 535;

    if(mouse->x + mouse->width > (int)h_res){
        mouse->x = (int)h_res - mouse->width -5;
    }else if(mouse->x < 0){
        mouse->x = 5;
    }else if(mouse->y + mouse->height > (int)v_res){
        mouse->y = (int)v_res - mouse->height - 5;
    }
    else if(mouse->y < 0){
        mouse->y = 5;
    }
    

    packet_struct.delta_x = 0;
    packet_struct.delta_y = 0;
}

void (organize_packets)(){
    if(!synced){
        if((packet & BIT(3)) == 0){
            printf("Mouse out of sync! Click\n");
            return;
        }
        else{
            synced = true;
        }
    }

    if(mouse_counter==0){
        packet_struct.bytes[0] = packet;
        mouse_counter++;
        return;
    }

    if(mouse_counter==1){
        packet_struct.bytes[1] = packet;
        mouse_counter++;
        return;
    }

    if(mouse_counter==2){
        packet_struct.bytes[2] = packet;
        mouse_counter = 0;

        if((packet_struct.bytes[0] & BIT(7))){
            packet_struct.y_ov = true;
        }
        else{
            packet_struct.y_ov = false;
        }

        if((packet_struct.bytes[0] & BIT(6))){
            packet_struct.x_ov = true;
        }
        else{
            packet_struct.x_ov = false;
        }

        if((packet_struct.bytes[0] & BIT(5))){
            packet_struct.delta_y = packet_struct.bytes[2] - 256;
        }
        else{
            packet_struct.delta_y = packet_struct.bytes[2];
        }

        if((packet_struct.bytes[0] & BIT(4))){
            packet_struct.delta_x = packet_struct.bytes[1] - 256;
        }
        else{
            packet_struct.delta_x = packet_struct.bytes[1];
        }

        if((packet_struct.bytes[0] & BIT(2))){
            packet_struct.mb = true;
        }
        else{
            packet_struct.mb = false;
        }

        if((packet_struct.bytes[0] & BIT(1))){
            packet_struct.rb = true;
        }
        else{
            packet_struct.rb = false;
        }

        if((packet_struct.bytes[0] & BIT(0))){
            packet_struct.lb = true;
        }
        else{
            packet_struct.lb = false;
        }
        
        update_mouse_pos();

    }
}

int(write_argument_to_mouse)(uint8_t argument){

    uint8_t stat;
    uint8_t acknowledgment;

    while(true){
        if(util_sys_inb(STATUS_REGISTER, &stat)!= 0) return 1;
        if((stat & IN_BUFF_BIT) == 0) {
            if (sys_outb(IN_BUF_COMMANDS, COMMAND_TO_MOUSE)) return 1;
            break;
        }
    }

    while(true){
        if(util_sys_inb(STATUS_REGISTER, &stat)!= 0) return 1;
        if((stat & IN_BUFF_BIT) == 0) {
            if (sys_outb(IN_BUF_ARGS, argument)) return 1;
            if(util_sys_inb(OUT_BUF,&acknowledgment)) return 1;
            if(acknowledgment == ACK) return 0;
            if(acknowledgment == NACK){
                return write_argument_to_mouse(argument);
            }
            if(acknowledgment == ERROR){
                return 1;
            }
        }
    }
}

int(enable_mouse_data)(){

  if (write_argument_to_mouse(SET_STREAM_MODE)) return 1;
  if (write_argument_to_mouse(ENABLE_MOUSE_DATA)) return 1;

  return 0;
}

int(disable_mouse_data)(){

  if (write_argument_to_mouse(DISABLE_MOUSE_DATA)!= 0) return 1;
  if (write_argument_to_mouse(SET_STREAM_MODE)!= 0) return 1;
  return 0;
}
