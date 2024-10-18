#include<common.h>
#include<linux/fb.h>
#include"fb_ctrl.h"
typedef struct fb_var_screeninfo fb_var_screeninfo;
static void print_lcd_info(int fd){
    static fb_var_screeninfo var;
    if(ioctl(fd, FBIOGET_VSCREENINFO, &var)){
        printf("Can't get var.\n");
        exit(0);
    }
    printf("========== lcd_info ==========\n");
    printf("x:%d y:%d\n",var.xres,var.yres);
    printf("bpp:0x%x\n",var.bits_per_pixel);
    printf("bit_red:0x%x    bit_green0x%x   bit_blue:0x%x\n",\
          var.red.length,   var.green.length,var.blue.length);
}

static int open_framebuffer(){
    int fd_fb = open("/dev/fb0",O_RDWR);
    if(fd_fb < 0){
        printf("/dev/fb0 open fail./n");
        exit(0);
    }
    return fd_fb;
}

int LCD_init(){
    int fd_fb = open_framebuffer();
    
    print_lcd_info(fd_fb);
    
    
    return fd_fb;
}

