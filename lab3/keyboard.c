#include "keyboard.h"

int hook_id_global_kbd = 0;
uint8_t scancode = 0;
u_int8_t size = 1;
u_int8_t scancode_array[2];

int (kbc_subscribe_int)(uint8_t * bit_no){
  int bit_no_int = *bit_no;
  if(bit_no == NULL){
    return 1;
  }
  if(sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&bit_no_int)!=0){
    return 1;
  }
  hook_id_global_kbd = bit_no_int;
  return 0;
}

int (kbc_unsubscribe_int)(){
  if(sys_irqrmpolicy(&hook_id_global_kbd)!=0){
    return 1;
  }
  return 0;
}

void (kbc_ih)(){
  uint8_t status;
  if(util_sys_inb(STATUS_REGISTER,&status)!=0){return;}

  if(util_sys_inb(OUT_BUF,&scancode)!=0){return;}

  if((status & (KBC_PAR_ERROR|KBC_TO_ERROR))!=0){
    scancode=0;
    return;
  }
}

int (kbc_print_scancode)(){
  
  bool make = !(scancode & BIT(7));
  
  if(scancode==0xE0){
    scancode_array[0] = scancode;
    size = 2;
    return 0;
  }
  if(size==2){
    scancode_array[1] =scancode;
    if(kbd_print_scancode(make,size,scancode_array)!=0){return 1;};
    size = 1;
    return 0;
  }
  else{
    scancode_array[0] = scancode;
    if(kbd_print_scancode(make,size,scancode_array)!=0){return 1;};
    return 0;
  }
}


int (readStatus)(){
  uint8_t status;

  while (true) {
    if (util_sys_inb(STATUS_REGISTER, &status) != 0) return 1;
    
    if ((status & OUT_BUFF_BIT) && !(status & AUX)){
      if(util_sys_inb(OUT_BUF, &scancode) != 0) return 1;
      if ((status & (KBC_PAR_ERROR | KBC_TO_ERROR)) != 0) return 1;
      return 0;
    }
  }
}
