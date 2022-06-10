#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include "sprites.h"


#define MIN_SLOPE 1.0

/**
 * Enum para lidar com o reconhecimento de um padrão
 */
typedef enum { INIT, DRAW_UP, DRAW_DOWN,
DETECTED } state_t;

/**
 * Enum para representar um evento de rato
 */
typedef enum { RBDOWN, RBUP, LBDOWN, LBUP, MBDOWN,
MBUP, MOVE } event_type_t;

/**
 * Estrutura que representa um evento do rato
 */
typedef struct {
event_type_t type;
uint8_t moveX;
uint8_t moveY;
bool lbdown, rbdown, mbdown;
} mouse_event_t;

/**
 * Subscrever as interrupções
 * @param bit_no hook_id
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(mouse_subscribe)(uint8_t * bit_no);

/**
 * Cancelar a subscrição
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(mouse_unsubscribe)();

/**
 * Interrupt handler - lê os packets do rato
 */
void(mouse_ih)();

/**
 * Organiza os bytes lidos em packets
 */
void (organize_packets)();

/**
 * Escreve um argumento para o rato
 * @param argument Argumento a escrever
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(write_argument_to_mouse)(uint8_t argument);

/**
 * Ativa o mouse data
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(enable_mouse_data)();

/**
 * Desativa o mouse data
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int(disable_mouse_data)();

/**
 * Atualiza a posição do cursor/mira de acordo com o packet lido
 */
void (update_mouse_pos)();

#endif
