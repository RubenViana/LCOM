#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb!=NULL){
    *lsb = val;
    return 0;
  }
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb!=NULL){
    *msb = (val >> 8);
    return 0;
  }
  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {

  if (value!=NULL){
    u_int32_t x;
    sys_inb(port, &x);
    *value = x;
    return 0;
  }
  return 1;
}
