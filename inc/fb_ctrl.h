#ifndef __FB_CTRL__
#define __FB_CTRL__
    #include<common.h>
    #include<linux/fb.h>
    typedef struct fb_var_screeninfo fb_var_screeninfo;

    int open_framebuffer();
    fb_var_screeninfo print_lcd_info(int fd);
#endif
