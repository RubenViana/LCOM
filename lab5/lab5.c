// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "videocard.h"
#include "keyboard.h"

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you


extern uint8_t scancode;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode)==NULL){
    printf("\t vg_init(): error ");
    return 1;
  }
  sleep(delay);
  if(vg_exit()!=0){
    printf("\t vg_exit(): error ");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if(vg_init(mode)==NULL){
    printf("\t vg_init(): error ");
    return 1;
  }
  
  vg_draw_rectangle(x,y,width,height,color);
  double_buffer(); 

  int ipc_status;
  bool done = false;
  u_int8_t bit_no = 1;
  u_int32_t irq_set = BIT(bit_no);
  message msg;
  kbc_subscribe_int(&bit_no);
  while (!done) { 
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
            if (scancode == 0x81) done = true;
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();

  if(vg_exit()!=0){
    printf("\t vg_exit(): error ");
    return 1;
  }
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int ipc_status;
  bool done = false;
  u_int8_t bit_no = 1;
  u_int32_t irq_set = BIT(bit_no);
  message msg;
   
   if(vg_init(mode)==NULL){
    printf("\t vg_init(): error ");
    return 1;
  }

  vg_draw_pattern(mode,no_rectangles,first,step);
  
  double_buffer(); 

  kbc_subscribe_int(&bit_no);
  while (!done) { 
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
            if (scancode == 0x81) done = true;
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();

  if(vg_exit()!=0){
    printf("\t vg_exit(): error ");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int ipc_status;
  bool done = false;
  u_int8_t bit_no = 1;
  u_int32_t irq_set = BIT(bit_no);
  message msg;
  int mode = 0x105;
   
   if(vg_init(mode)==NULL){
    printf("\t vg_init(): error ");
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);
  draw_sprite(img,sprite,x,y);

  double_buffer(); 

  kbc_subscribe_int(&bit_no);
  while (!done) { 
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
            if (scancode == 0x81) done = true;
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();

  if(vg_exit()!=0){
    printf("\t vg_exit(): error ");
    return 1;
  }
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  timer_set_frequency(0,30);

  bool done = false;
  int ipc_status;
  u_int8_t bit_no_kb = 1;
  u_int32_t irq_set_kb = BIT(bit_no_kb);
  u_int8_t bit_no_t = 0;
  u_int32_t irq_set_t = BIT(bit_no_t);
  message msg;
  int mode = 0x105;
   
   if(vg_init(mode)==NULL){
    printf("\t vg_init(): error ");
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);
  
  draw_sprite(img, sprite, xi, yi);

  kbc_subscribe_int(&bit_no_kb);
  timer_subscribe_int(&bit_no_t);

  while (scancode!=0x81) { 
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             
          if (msg.m_notify.interrupts & irq_set_kb) {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & irq_set_t) {
            timer_int_handler();
            printf("%d\n",counter);
            if (!done) {
                if(speed < 0 && counter % (-speed) == 0){
                    if (xi == xf) {
                    vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                    yi = yi + 1;
                    if (yi > yf) {
                      yi = yf;
                    }
                    draw_sprite(img, sprite, xi, yi);
                    if (yi == yf)
                      done = true;
                  }
                  else if (yi == yf) {
                    vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                    xi = xi + 1;
                    if (xi > xf) {
                      xi = xf;
                    }
                    draw_sprite(img, sprite, xi, yi);
                    if (xi == xf)
                      done = true;
                  }
                }
                else if(speed > 0){
                  if (xi == xf) {
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  yi = yi + speed;
                  if (yi > yf) {
                    yi = yf;
                  }
                  draw_sprite(img, sprite, xi, yi);
                  if (yi == yf)
                    done = true;
                }
                else if (yi == yf) {
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  xi = xi + speed;
                  if (xi > xf) {
                    xi = xf;
                  }
                  draw_sprite(img, sprite, xi, yi);
                  if (xi == xf)
                    done= true;
                }
              }
              double_buffer(); 
            }
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe_int();
  timer_unsubscribe_int();

  if(vg_exit()!=0){
    printf("\t vg_exit(): error ");
    return 1;
  }
  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
