/*
 * PROJECT:   LVGL PC Simulator using Visual Studio
 * FILE:      LVGL.Simulator.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "resource.h"

#if _MSC_VER >= 1200
 // Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/win32drv/win32drv.h"
//#include "mycode/cam.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

#include <stdio.h>

//typedef struct tagBITMAPFILEHEADER
//{
//    UINT16 bfType;
//    DWORD bfSize;
//    UINT16 bfReserved1;
//    UINT16 bfReserved2;
//    DWORD bfOffBits;
//} BITMAPFILEHEADER;
//
//typedef struct tagBITMAPINFOHEADER
//{
//    DWORD       biSize;
//    LONG        biWidth;
//    LONG        biHeight;
//    WORD        biPlanes;
//    WORD        biBitCount;
//    DWORD       biCompression;
//    DWORD       biSizeImage;
//    LONG        biXPelsPerMeter;
//    LONG        biYPelsPerMeter;
//    DWORD       biClrUsed;
//    DWORD       biClrImportant;
//} BITMAPINFOHEADER;
BITMAPFILEHEADER f1_Header;
BITMAPINFOHEADER f1_Info;
struct img_data
{
    lv_obj_t* obj;
    uint16_t x;
    uint16_t y;
};
void move_handle(lv_timer_t* e)
{
    img_data* obj = (img_data*)(e->user_data);
    //static int x;
    (800 > obj->x) ? obj->x += 10 : obj->x = 300;
    lv_obj_set_pos(obj->obj, obj->x, obj->y);
}
void move_handle2(lv_timer_t* e)
{
    img_data* obj = (img_data*)(e->user_data);
    //static int x;
    (800 > obj->x) ? obj->x += 10 : obj->x = 300;
    lv_obj_set_pos(obj->obj, obj->x, obj->y);
}

void ack_handle(void)
{
    LV_LOG_USER("\r\n work");
}

#include "mycode/lcd.h"
struct yoga
{
    lv_obj_t* box;
    lv_obj_t* btn;
    uint16_t boxsize;
    uint16_t btnsize;
    uint16_t limit;
    lv_point_t val;
}yoga1;
#define valuelimit(n,a,b) (n<a)?n=(a):n;(n>b)?n=(b):n;
void yoga_event(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    yoga* t = (yoga*)e->user_data;
    if (obj == t->btn)
    {
        if (code == LV_EVENT_PRESSING)
        {
            lv_point_t click_pos, pos;
            lv_area_t click_area;
            lv_indev_get_point(lv_indev_get_act(), &click_pos);
            lv_obj_get_click_area(t->box, &click_area);
            pos.x = (click_pos.x - click_area.x1);
            pos.y = (click_pos.y - click_area.y1);
            valuelimit(pos.x, t->limit, t->boxsize - t->limit);
            valuelimit(pos.y, t->limit, t->boxsize - t->limit);
            t->val.x = pos.x;
            t->val.y = pos.y;
            /*if (pos.x < t->btnsize / 2 && pos.y < t->btnsize / 2)
            {
                pos.x = t->btnsize / 2;
                pos.y = t->btnsize / 2;
            }*/
            pos.x = t->boxsize / 2 - pos.x;
            pos.y = t->boxsize / 2 - pos.y;
            //(pos.x > 0 + t->btnsize / 2 && pos.x < t->boxsize- t->btnsize / 2)?1:
            lv_obj_align(t->btn, LV_ALIGN_CENTER, -pos.x, -pos.y);
            //lv_obj_set_pos(t->btn, click_pos.x - click_area.x1, click_pos.y - click_area.y1);
            LV_LOG_USER("\r\n %d,%d", pos.x, pos.y);

        }
        else
        {
            if (code == LV_EVENT_CLICKED)
            {
                lv_obj_align(t->btn, LV_ALIGN_CENTER, 0, 0);
            }
        }
    }
}
void yoga_creat(lv_obj_t* par)
{
    yoga1.boxsize = 100;
    yoga1.btnsize = 30;
    yoga1.limit = yoga1.btnsize-10;
    lv_obj_t* obj = lv_obj_create(par);
    //lv_obj_remove_style_all(obj);
    
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(obj, yoga1.boxsize, yoga1.boxsize);
    lv_obj_set_style_radius(obj, yoga1.btnsize, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* btn = lv_btn_create(obj);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(btn, yoga1.btnsize, yoga1.btnsize);
    lv_obj_set_style_radius(btn, yoga1.btnsize, 0);
    yoga1.box = obj;
    yoga1.btn = btn;
    //lv_obj_move_foreground(btn);
    //lv_obj_set_parent()
    lv_obj_add_event_cb(btn, yoga_event, LV_EVENT_ALL, (void*)&yoga1);
}



int main()
{
    lv_init();

    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        680,
        680,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }
    lv_fs_win32_init();
    lv_win32_add_all_input_devices_to_group(NULL);
    /*lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, "C:/Users/pzh/Desktop/t4.bmp", LV_FS_MODE_RD);
    if (res == LV_RES_OK) {

     }
     uint8_t header[56];
     lv_fs_read(&f, header, 56, NULL);
     memcpy(&f1_Header, header, 14);
     memcpy(&f1_Info, header+ 14, 40);
     lv_fs_close(&f);*/
#if 0
    img_data img1, img2;
    memset(&img1, 0, sizeof img1);
    memset(&img2, 0, sizeof img2);
    img1.obj = lv_img_create(lv_scr_act());
    lv_img_set_src(img1.obj, "C:/Users/pzh/Desktop/t6.bmp");
    img2.obj = lv_img_create(lv_scr_act());
    lv_img_set_src(img2.obj, "C:/Users/pzh/Desktop/t2_index16.bmp");
    img2.x = 400;
    img2.y = 300;
    lv_timer_create(move_handle, 1000, (void*)&img1);
    lv_timer_create(move_handle2, 1000, (void*)&img2);
    lv_btn_create(lv_scr_act());

#endif // 0
    LCD_INIT();
    //lv_timer_create((lv_timer_cb_t)ack_handle, 1000, 0);
    while (!lv_win32_quit_signal)
    {
        lv_task_handler();
        Sleep(1);
    }

    return 0;
}
