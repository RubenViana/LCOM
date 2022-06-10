#ifndef VIDEOCARD_H_
#define VIDEOCARD_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>
#include "sprites.h"

/**
 * Inicializa o modo de vídeo do minix
 * @param mode Modo de vídeo a ser usado
 * @return Memória de vídeo mapeada
 */
void *(vg_init)(uint16_t mode);

/**
 * Desenha um pixel para o buffer
 * @param x Posição x
 * @param y Posição y
 * @param color Cor
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * Desenha uma linha horizontal para o buffer
 * @param x Posição x inicial
 * @param y Posição y
 * @param len Tamanho
 * @param color Cor
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * Desenha um retângulo para o buffer
 * @param x Posição x inicial
 * @param y Posição y inicial
 * @param width Largura
 * @param height Altura
 * @param color Cor
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * Desenha um padrão para o buffer
 * @param mode Modo de vídeo
 * @param no_rectangles Número de retângulos
 * @param first Primeira cor
 * @param step Salto entre cores
 * @return 0 a indicar sucesso ou 1 a indicar falha
 */
int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

/**
 * Desenha um sprite do projeto para o buffer
 * @param sprite Sprite a desenhar
 */
void draw_sprite_proj (Sprite sprite);

/**
 * Desenha uma xmp image para o buffer
 * @param img Pixmap
 * @param sprite Sprite
 * @param x Posição x inicial
 * @param y Posição y inicial
 */
void(draw_sprite)(xpm_image_t img,uint8_t *sprite,int x, int y);

/**
 * Copia o conteúdo do buffer para a memória de vídeo mapeada
 */
void (double_buffer)();

#endif
