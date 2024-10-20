#include<common.h>
#include"fb_ctrl.h"
#include<linux/fb.h>
#include <sys/mman.h>
#include<string.h>
#include <unistd.h>
#include "ascii.h"
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;
fb_var_screeninfo var;
int  map_framebuffer(int fd){
    line_width = var.xres * var.bits_per_pixel / 8;
    pixel_width = var.bits_per_pixel / 8;
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

    fb_base = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb_base == (unsigned char *)-1)
    {
        printf("can't mmap\n");
        return -1;
    }


	
    return 0;
}

static void regroup_565(u_int16_t *red, u_int16_t *green, u_int16_t *blue, u_int32_t *color){
    *red    = (*color >> 16) &0xff;
    *green  = (*color >> 8)  &0xff;
    *blue   =  *color        &0xff;
    *color  = ((*red & 0x1f) << 11) | ((*green & 0x3f) << 5) | (*blue & 0x1f);
}
/* 
    color: 
        透明度:  8b  
        red:    8b 
        green:  8b 
        blue:   8b
*/
enum{
    BPP_8B  = 8,
    BPP_16B = 16,
    BPP_32B = 32,
}BPP_BIT;
void lcd_put_pixel(int x, int y, u_int32_t color){
    u_int8_t *pen_8  = fb_base + y*line_width + x*pixel_width;
    u_int16_t*pen_16 = (u_int16_t*)pen_8;
    u_int32_t*pen_32 = (u_int32_t*)pen_8;

    u_int16_t red, green, blue;

    switch(var.bits_per_pixel){
        case BPP_8B:{
            *pen_8 = color;
            break;
        }
        case BPP_16B:{
            if(var.red.length==5 && var.green.length==6 && var.blue.length==5){
                regroup_565(&red, &green, &blue, &color);
                *pen_16 = color;
            }
            break;
        }
        case BPP_32B:{
            *pen_32 = color;
            break;
        }
        default:{
            printf("Can't surpport %dbpp\n",var.bits_per_pixel);
            break;
        }
    }
}



static void put_ascii(int x,int y,u_int8_t c){
    u_int8_t *dots = (u_int8_t*) &fontdata_8x16[c*16];
    u_int8_t byte;
    for(int i=0;i<16;i++){
        byte = dots[i];
        for(int idx = 7;~idx;idx--){
            if(byte & (1<< idx))lcd_put_pixel(x+7-idx,y+i,0xffffff);
            else lcd_put_pixel(x+7-idx,y+i,0);
        }
    }
}
void printf_ascii(char *str){
    char buffer[1024];
    sprintf(buffer,"%s",str);
    static int x = 0;
    static int y = -16;
    y+=16;
    x = 0;
    int len = strlen(buffer);
    for(int i=0;i<len;i++,x+=8){
        put_ascii(x,y,str[i]);
    }   
}
int main(){
    int fd_fb = open_framebuffer();
    var = print_lcd_info(fd_fb);
    map_framebuffer(fd_fb);

    memset(fb_base, 0x00, screen_size);
    // for (int i = 0; i < 10; i++){
        
    // }
    // put_ascii(30,20,'a');
    printf_ascii("abdcdeg");
    printf_ascii("ABCDEFG");
    
    while(1);
    close(fd_fb);
    return 0;
}