// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"

extern uint32_t total_packets;
extern bool reading_error;
extern int mouse_counter;
extern int counter;
extern bool gesture_detected;
extern mouse_event_t event;
extern bool packet_read;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    if(enable_mouse_data()!=0){
        return 1;
    }
    int ipc_status;
    u_int8_t bit_no = 1;
    u_int32_t irq_set = BIT(bit_no);
    message msg;
    mouse_subscribe(&bit_no);

    while (total_packets < cnt) { 
        int r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { 
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:                             
                if (msg.m_notify.interrupts & irq_set) {
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
                default: break;
            }
        }
    }
    mouse_unsubscribe();

    if (disable_mouse_data()){  
        return 1;
    }
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {

    if(enable_mouse_data()!=0){
        return 1;
    }
    int ipc_status;
    u_int8_t bit_no_mouse = 1;
    u_int32_t irq_set_mouse = BIT(bit_no_mouse);
    u_int8_t bit_no_timer = 0;
    u_int32_t irq_set_timer = BIT(bit_no_timer);
    message msg;
    mouse_subscribe(&bit_no_mouse);
    timer_subscribe_int(&bit_no_timer);

    while (counter < idle_time * 60) { 
        int r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { 
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:                             
                if (msg.m_notify.interrupts & irq_set_mouse) {
                    counter = 0;
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
                if (msg.m_notify.interrupts & irq_set_timer) {
                    timer_int_handler();
                }
                break;
                default: break;
            }
        }
    }
    mouse_unsubscribe();

    if (disable_mouse_data()){  
        return 1;
    }

    timer_unsubscribe_int();
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    if(enable_mouse_data()!=0){
        return 1;
    }
    int ipc_status;
    u_int8_t bit_no = 1;
    u_int32_t irq_set = BIT(bit_no);
    message msg;
    mouse_subscribe(&bit_no);

    while (!gesture_detected) { 
        int r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { 
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:                             
                if (msg.m_notify.interrupts & irq_set) {
                    mouse_ih();
                    if (reading_error == true && mouse_counter != 2) {
                        continue;
                    }
                    else if (reading_error == true && mouse_counter == 2) {
                        reading_error = false;
                    }
                    else{
                        organize_packets();
                        if(packet_read){
                            printf("%d\n",event.type);
                            mouse_check_pattern(event,x_len,tolerance);
                        }
                    }
                }
                break;
                default: break;
            }
        }
    }
    mouse_unsubscribe();

    if (disable_mouse_data()){  
        return 1;
    }
    return 0;
}

