#ifndef _MY_LCD_H
#define _MY_LCD_H
#include "../lvgl/lvgl.h"


//class frema_info
//{
//public:
//    frema_info();
//    ~frema_info();
//    uint16_t width;
//    uint16_t height;
//    uint16_t fps;
//private:
//    
//    lv_color_t* buf;
//};
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
void LCD_INIT();
#endif
