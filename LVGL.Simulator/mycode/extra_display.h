#ifndef MY_CODE_H
#define MY_CODE_H
#include "LVGL.Simulator/lvgl/lvgl.h"
//void Extra_display_reg_init();

typedef enum
{
    wait,
    drawing,
}scr_draw_t;

struct display_info
{
    uint32_t w;
    uint16_t h;
    lv_color_t* frema_buf;
    lv_point_t indev;
    uint8_t valid;              //是否在显示状态
};

extern struct display_info display1;
void Extra_display_init(uint16_t fps);
void Ex_display_refulsh(lv_timer_t* e);
void display_set_size(uint32_t w, uint32_t h, lv_color_t* e);
uint8_t display_get_vaild(display_info* d);
#endif
