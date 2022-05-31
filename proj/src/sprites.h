#ifndef SPRITES_H_
#define SPRITES_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>

typedef struct {
    int x, y; // current position
    int width, height; // dimensions
    uint8_t *map; // the pixmap
} Sprite;

Sprite* create_sprite(xpm_map_t map,int x, int y);

void destroy_sprite(Sprite *sp);

#endif
