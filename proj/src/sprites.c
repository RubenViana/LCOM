#include <lcom/lcf.h>

#include "sprites.h"

Sprite* create_sprite(const char *pic[],int x, int y) {

    Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
    xpm_image_t img;
    if (sp == NULL){
        return NULL;
    }
    // read the sprite pixmap
    sp->map = xpm_load(pic, XPM_INDEXED, &img);
    if (sp->map == NULL) {
        free(sp);
        return NULL;
    }
    sp->width = img.width;
    sp->height = img.height;
    sp->x = x;
    sp->y = y;
    return sp;
}