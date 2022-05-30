#ifndef SPRITE_H_
#define SPRITE_H_

#include <lcom/lcf.h>

typedef struct {
    int x, y; // current position
    int width, height; // dimensions
    uint8_t *map; // the pixmap
} Sprite;

/**
* Creates a new sprite with pixmap "pic",
* with specified position (within the
* screen limits) and speed;
* Does not draw the sprite on the screen.
*
* @param pic lines of strings, same as xpm_map_t (has const protectors)
* @return NULL on invalid pixmap.
*/
Sprite* (create_sprite)(const char *pic[],int x, int y);

void destroy_sprite(Sprite *sp);

#endif
