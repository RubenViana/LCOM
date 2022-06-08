#include "sprites.h"

Sprite* create_sprite(xpm_map_t map,int x, int y, int xSpeed, int ySpeed) {

    Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
    enum xpm_image_type type = XPM_8_8_8_8;
    xpm_image_t img;
    if (sp == NULL){
        return NULL;
    }
    // read the sprite pixmap
    sp->map = xpm_load(map, type, &img);
    if (sp->map == NULL) {
        free(sp);
        return NULL;
    }
    sp->width = img.width;
    sp->height = img.height;
    sp->x = x;
    sp->y = y;
    sp->xSpeed = xSpeed;
    sp->ySpeed = ySpeed;
    return sp;
}

void destroy_sprite(Sprite *sp){
    if(sp ==NULL){
        return;
    }
    if(sp->map){
        free(sp->map);
    }
    free(sp);
}

