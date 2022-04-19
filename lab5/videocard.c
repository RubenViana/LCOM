#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "videocard.h"

static void *video_mem;  

static unsigned h_res;	       
static unsigned v_res;	        
static unsigned bits_per_pixel;
static unsigned bytes_per_pixel;


void *(vg_init)(uint16_t mode){   

struct minix_mem_range mr;

unsigned int vram_base; 
unsigned int vram_size;  
int r;				    

vbe_mode_info_t record;

vbe_get_mode_info(mode,&record);

h_res = record.XResolution;

v_res = record.YResolution;

bits_per_pixel = record.BitsPerPixel;

vram_base = record.PhysBasePtr;

if(bits_per_pixel % 8 != 0){
  bits_per_pixel+=7;
}

bytes_per_pixel = (bits_per_pixel/8);

vram_size = h_res * v_res * bytes_per_pixel;

mr.mr_base = (phys_bytes) vram_base;	
mr.mr_limit = mr.mr_base + vram_size;  

if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
   panic("sys_privctl (ADD_MEM) failed: %d\n", r);
   return NULL;
}

/* Map memory */

video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

if(video_mem == MAP_FAILED){
   panic("couldn't map video memory");
   return NULL;
}

reg86_t r86;

memset(&r86, 0, sizeof(r86));	/* zero the structure */

r86.intno = 0x10; 
r86.ah = 0x4F;    
r86.al = 0x02;

r86.bx = mode | BIT(14);

if( sys_int86(&r86) != OK ) {
  printf("\tvg_exit(): sys_int86() failed \n");
  return NULL;
}  

if(r86.al!=0x4F){
  printf("\tFunction not supported \n");
  return NULL;
}
else{
  if(r86.ah!=0x00){
    printf("\tFunction not successful  \n");
    return NULL;
  }
}

return video_mem;

}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){

  memcpy(((char*)(video_mem)) + (y * h_res + x) * bytes_per_pixel,&color,bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

  for (uint16_t i = 0; i < len; i++)
  {
    vg_draw_pixel(x + i,y,color);
  }

  return 0;
  
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (uint16_t i = 0; i < height; i++)
  {
    vg_draw_hline(x,y + i,width,color);
  }    
  return 0;                    
}
