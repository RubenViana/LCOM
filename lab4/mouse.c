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
uint32_t total_packets = 0;
bool reading_error = false;
bool gesture_detected = false;
mouse_event_t event;
bool packet_read = false;


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
        packet_read = false;
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

        total_packets++;
        mouse_print_packet(&packet_struct);
        
        if(packet_struct.delta_x!=0 || packet_struct.delta_y != 0){
            event.type = MOVE;
        }
        if(packet_struct.lb){
            event.type = LBDOWN;
        }
        if(packet_struct.rb){
            event.type = RBDOWN;
        }
        event.moveX = packet_struct.delta_x;
        event.moveY = packet_struct.delta_y;
        event.lbdown = packet_struct.lb;
        event.mbdown = packet_struct.mb;
        event.rbdown = packet_struct.rb;
        packet_read = true;
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


void (mouse_check_pattern)(mouse_event_t evt,uint8_t x_len,uint8_t tolerance) {
    static state_t pattern_state = INIT; // initial state; keep state
    static int lineXLen = 0; // line movement in X 
    switch (pattern_state) {
        case INIT:
            if ((evt.type == LBDOWN && !evt.rbdown && !evt.mbdown)
                    || (evt.type == MOVE && evt.lbdown && !evt.rbdown
                        && !evt.mbdown)) { 
                            lineXLen = 0; pattern_state = DRAW_UP;
                    } //else we stay on INIT
                    break;
        case DRAW_UP:
            if (evt.type == MOVE) {
                lineXLen += evt.moveX;
                if (!evt.lbdown && !evt.mbdown && evt.rbdown && lineXLen >= x_len) { // although a move type, we allow the button change
                    pattern_state = DRAW_DOWN; }
                else if (evt.moveX + tolerance <= 0 || evt.moveY + tolerance <= 0 || (evt.moveY + tolerance / evt.moveX + tolerance) < MIN_SLOPE || !evt.lbdown || evt.mbdown || (evt.rbdown && lineXLen < x_len)) { // note that lifting left button and pressing right is handled before
                        pattern_state = INIT; }
            } // evt.type == MOVE
            else if (evt.type == RBDOWN) { // change the line draw
                if (!evt.lbdown && !evt.mbdown && // it was a right click, but we're extra careful
                    evt.rbdown && lineXLen >= x_len) { 
                        lineXLen = 0;
                        pattern_state = DRAW_DOWN;
                }
                else { // does not comply, right click with other or length of UP too short
                    pattern_state = INIT; }
            } // evt.type == RBDOWN
            else { // other event type goes to restart
                pattern_state = INIT; }
        break; // from case DRAW_UP           

        case DRAW_DOWN: 
            if (evt.type == MOVE) {
                lineXLen += evt.moveX;
                if (lineXLen >= x_len && evt.rbdown && !evt.mbdown && !evt.rbdown) { 
                    pattern_state = DETECTED; }
                else if (evt.moveX + tolerance <= 0 || evt.moveY - tolerance >= 0 || (evt.moveY - tolerance / evt.moveX + tolerance) > -MIN_SLOPE || evt.lbdown || evt.mbdown || (!evt.rbdown && lineXLen < x_len)) { 
                        pattern_state = INIT; }
            }
            else {
                pattern_state = INIT; }
        break;

        case DETECTED:
            if( !evt.rbdown && !evt.mbdown && !evt.rbdown){
                gesture_detected = true;
            }
            else if(evt.rbdown && !evt.mbdown && !evt.lbdown){
                break;
            }
            else{
                pattern_state = INIT;
            }
        break;
    } 
}
