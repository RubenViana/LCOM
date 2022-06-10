#include <lcom/lcf.h>
#include <stdint.h>

int cnt = 0;

/**
 * Obter o byte menos significativo de um valor
 * @param val Valor
 * @param lsb Onde guardar o byte
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb!=NULL){
    *lsb = val;
    return 0;
  }
  return 1;
}

/**
 * Obter o byte mais significativo de um valor
 * @param val Valor
 * @param msb Onde guardar o byte
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb!=NULL){
    *msb = (val >> 8);
    return 0;
  }
  return 1;
}

/**
 * Permite a utilização de sys_inb com valores de 1 byte
 * @param port Porta de onde ler
 * @param value Onde guardar o valor
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (util_sys_inb)(int port, uint8_t *value) {

  if (value!=NULL){
    u_int32_t x;
    #ifdef LAB3
      cnt ++;
    #endif
    sys_inb(port, &x);
    *value = x;
    return 0;
  }
  return 1;
}
