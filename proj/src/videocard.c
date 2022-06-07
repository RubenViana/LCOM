#include "videocard.h"

static void *video_mem;  

char* buffer;

unsigned h_res;	       
unsigned v_res;	        
static unsigned bits_per_pixel;
static unsigned bytes_per_pixel;
static vbe_mode_info_t vbe_info;


void *(vg_init)(uint16_t mode){   

struct minix_mem_range mr;

unsigned int vram_base; 
unsigned int vram_size;  
int r;				    

vbe_mode_info_t record;


vbe_get_mode_info(mode,&record);

vbe_info = record;

h_res = record.XResolution;

v_res = record.YResolution;

bits_per_pixel = record.BitsPerPixel;

vram_base = record.PhysBasePtr;

if(bits_per_pixel % 8 != 0){
  bits_per_pixel+=7;
}

bytes_per_pixel = (bits_per_pixel/8);

vram_size = h_res * v_res * bytes_per_pixel;

buffer = (char *) malloc(vram_size);

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

void (double_buffer)(){
    memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){

  if(x >= h_res || y >= v_res){
    return 1;
  }

  //memcpy(((char*)(video_mem)) + (y * h_res + x) * bytes_per_pixel,&color,bytes_per_pixel);

  memcpy(buffer + (y * h_res + x) * bytes_per_pixel,&color,bytes_per_pixel);

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

int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  int x_rectangle = h_res / no_rectangles;
  int y_rectangle = v_res / no_rectangles;
  uint32_t color;
  uint32_t  R , G ,B;

  for (int row = 0; row < no_rectangles; row++){
    for (int col = 0; col < no_rectangles; col++){
      if(mode==0x105){
        color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
      }
      else{
        uint32_t redMask = 0XFFFFFFFF >> (32-vbe_info.RedMaskSize);
        R = (((first >> vbe_info.RedFieldPosition) & redMask)  + col * step) % (1 << vbe_info.RedMaskSize);

        uint32_t greenMask = 0XFFFFFFFF >> (32-vbe_info.GreenMaskSize);
        G = (((first >> vbe_info.GreenFieldPosition) & greenMask) + row * step) % (1 << vbe_info.GreenMaskSize);

        uint32_t blueMask = 0XFFFFFFFF >> (32-vbe_info.BlueMaskSize);
        B = (((first >> vbe_info.BlueFieldPosition) & blueMask) + (col + row) * step) % (1 << vbe_info.BlueMaskSize);

        color = 0 | (R << vbe_info.RedFieldPosition) | (G << vbe_info.GreenFieldPosition) | (B<<vbe_info.BlueFieldPosition);
        
      }
        
      vg_draw_rectangle(col*x_rectangle,row*y_rectangle,x_rectangle,y_rectangle,color);
    }
  }
  return 0;
}

void(draw_sprite)(xpm_image_t img,uint8_t *sprite,int x, int y){
  uint32_t color,red,green,blue,byte1;
  uint8_t* temp_sprite = sprite;
  for(int i = 0; i < img.height; i++){
    for(int j = 0; j < img.width; j++){
      byte1 = *(temp_sprite + 3) & 0xff;
      red = *(temp_sprite + 2) & 0xff;
      green = *(temp_sprite + 1) & 0xff;
      blue = *(temp_sprite) & 0xff;
      color = byte1 << 24 | red << 16 | green << 8 | blue;
      if(xpm_transparency_color(XPM_8_8_8_8) != color){
        vg_draw_pixel(x + j,i + y,color);
      }
      temp_sprite+=4;
    }
  }
}

void draw_sprite_proj (Sprite sprite){
  uint32_t color,red,green,blue,byte1;
  uint8_t* temp_sprite = sprite.map;
  for(int i = 0; i < sprite.height; i++){
    for(int j = 0; j < sprite.width; j++){
      byte1 = *(temp_sprite + 3) & 0xff;
      red = *(temp_sprite + 2) & 0xff;
      green = *(temp_sprite + 1) & 0xff;
      blue = *(temp_sprite) & 0xff;
      color = byte1 << 24 | red << 16 | green << 8 | blue;
      if(xpm_transparency_color(XPM_8_8_8_8) != color){
        vg_draw_pixel(sprite.x + j,i + sprite.y,color);
      }
      temp_sprite+=4;
    }
  }
}
