#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <lcom/lcf.h>
#include "i8042.h"
#include <stdint.h>

/**
 * Subscrever as interrupções
 * @param bit_no hook_id
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (kbc_subscribe_int)(uint8_t * bit_no);

/**
 * Cancelar a subscrição
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (kbc_unsubscribe_int)();

/**
 * Interrupt handler - lê o scancode
 */
void (kbc_ih)();

/**
 * Dá print ao scancode
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (kbc_print_scancode)();

/**
 * Lê o status do teclado
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (readStatus)();

#endif
