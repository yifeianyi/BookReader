#include<common.h>
#include"fb_ctrl.h"
#include<linux/fb.h>
#include <sys/mman.h>
#include<string.h>
#include <unistd.h>
#include "ascii.h"
#include <errno.h>
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
int fd_hzk16;
struct stat hzk_stat;
unsigned char *hzkmem;

int open_HZK16(){
    int fd = open("src/HZK16", O_RDONLY);
	if (fd < 0)
	{
		printf("can't open HZK16\n");
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("pwd: %s\n",cwd);
		return -1;
	}
    else printf("Open HZK16 finished.\n");
    
	if(fstat(fd, &hzk_stat))
	{
		printf("can't get fstat\n");
		return -1;
	}
    else printf("Get HZK16 fstat finished.\n");
    printf("\n");
    return fd;
}

int map_hzk16(int fd){
    hzkmem = (unsigned char *)mmap(NULL , hzk_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (hzkmem == MAP_FAILED)
	{
		fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        close(fd_hzk16);
		return -1;
	}
    return 0;
}

//16*16点阵
void put_hzk16(int x, int y, u_int8_t *str){
    u_int8_t area = str[0] - 0xA1;
    u_int8_t offset = str[1] - 0xA1;
    u_int8_t *base = hzkmem + (area *94 + offset) *32;

    for(int i=0;i<16;i++){
        for(int j=0;j<2;j++){
            u_int8_t byte = base[i*2 + j];
            for(int b=7; ~b; b--){
                u_int32_t color = ((byte >> b) & 1)? 0xffffff : 0;
                lcd_put_pixel(x + j*8 + 7 - b, y+i, color);
            }
        }
    }
}
int main(){
    int fd_fb = open_framebuffer();
    var = print_lcd_info(fd_fb);
    map_framebuffer(fd_fb);
    printf("\n");
//============================================================
    memset(fb_base, 0x00, screen_size);

//================打印 ASCII 点阵==============================
    printf_ascii("abdcdeg");
    printf_ascii("ABCDEFG");

//================ HZK16 ======================================
    fd_hzk16 = open_HZK16();
    

    map_hzk16(fd_hzk16);
    

    unsigned char str[] = "中";
    printf("GBK2312 code: %02x %02x\n",str[0],str[1]);
    put_hzk16(var.xres/2,var.yres/2,str);

    while(1);
    close(fd_fb);
    return 0;
}