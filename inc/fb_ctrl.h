#ifndef __FB_CTRL__
#define __FB_CTRL__
    #include<common.h>
    #include<linux/fb.h>
    typedef struct fb_var_screeninfo fb_var_screeninfo;
    enum{
        BPP_8B  = 8,
        BPP_16B = 16,
        BPP_32B = 32,
    }BPP_BIT;

    int open_framebuffer();
    fb_var_screeninfo print_lcd_info(int fd);
#endif
