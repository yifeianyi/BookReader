#include<common.h>
#include"fb_ctrl.h"
#include<linux/fb.h>
#include <sys/mman.h>
#include<string.h>
#include <unistd.h>
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;
int  map_framebuffer(fb_var_screeninfo var,int fd){
    line_width = var.xres * var.bits_per_pixel / 8;
    pixel_width = var.bits_per_pixel / 8;
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

    fb_base = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb_base == (unsigned char *)-1)
    {
        printf("can't mmap\n");
        return -1;
    }

    // for(int i=0;i <= 0xff;i++){
    //     sleep(1);
    //     memset(fb_base, i, screen_size);
    // }

	
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
void lcd_put_pixel(fb_var_screeninfo var,int x, int y, u_int32_t color){
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

int main(){
    int fd_fb = open_framebuffer();
    fb_var_screeninfo var = print_lcd_info(fd_fb);
    map_framebuffer(var,fd_fb);

    memset(fb_base, 0xff, screen_size);
    for (int i = 0; i < 100; i++){
        lcd_put_pixel(var,var.xres/2+i, var.yres/2, 0xFF0000);
        lcd_put_pixel(var,var.xres/2, var.yres/2+i, 0xFF0000);
        lcd_put_pixel(var,var.xres/2-i, var.yres/2, 0xFF0000);
        lcd_put_pixel(var,var.xres/2, var.yres/2-i, 0xFF0000);

        lcd_put_pixel(var,var.xres/2+i, var.yres/2+i, 0xFF0000);
        lcd_put_pixel(var,var.xres/2-i, var.yres/2+i, 0xFF0000);
        lcd_put_pixel(var,var.xres/2-i, var.yres/2-i, 0xFF0000);
        lcd_put_pixel(var,var.xres/2+i, var.yres/2-i, 0xFF0000);
        // sleep(1);
        // memset(fb_base, i, screen_size);
    }

    while(1)

    close(fd_fb);
    return 0;
}