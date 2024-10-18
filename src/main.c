#include<common.h>
#include"fb_ctrl.h"
#include<linux/fb.h>
#include <sys/mman.h>

int main(){
    int fd_fb = LCD_init();

    // //显存映射
    // int line_wid = var.xres * var.bits_per_pixel/8;
    // int screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
    // int pixel_width = var.bits_per_pixel / 8;
    // u_int8_t* fb_base = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);

    //======================================================================================


    
    close(fd_fb);
    return 0;
}