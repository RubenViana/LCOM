#ifndef SPRITES_H_
#define SPRITES_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>

/**
 * Estrutura que representa um sprite
 */
typedef struct {
    int x, y; // current position
    int width, height; // dimensions
    uint8_t *map; // the pixmap
    int xSpeed, ySpeed;
} Sprite;

/**
 * Cria um sprite a partir de um pixmap
 * @param map Pixmap da imagem
 * @param x Posição x
 * @param y Posição y
 * @param xSpeed Velocidade em x
 * @param ySpeed Velocidade em y
 * @return Sprite criado
 */
Sprite* create_sprite(xpm_map_t map,int x, int y, int xSpeed, int ySpeed);

/**
 * Destrói sprites quando não são mais necessários
 * @param sp Sprite a destruir
 */
void destroy_sprite(Sprite *sp);

#endif
