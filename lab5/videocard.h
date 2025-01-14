#include <lcom/lcf.h>

void *(vg_init)(uint16_t mode);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

void(draw_sprite)(xpm_image_t img,uint8_t *sprite,int x, int y);

void (double_buffer)();

