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

    for(int i=0;i <= 0xff;i++){
        sleep(1);
        memset(fb_base, i, screen_size);
    }

	
    return 0;
}
int main(){
    int fd_fb = open_framebuffer();
    fb_var_screeninfo var = print_lcd_info(fd_fb);
    map_framebuffer(var,fd_fb);

    close(fd_fb);
    return 0;
}