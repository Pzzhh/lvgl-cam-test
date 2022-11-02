﻿/*
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
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/win32drv/win32drv.h"

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
int main()
{
    lv_init();

    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        800,
        480,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }
    lv_fs_win32_init();
    lv_win32_add_all_input_devices_to_group(NULL);
    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, "C:/Users/pzh/Desktop/t3.bmp", LV_FS_MODE_RD);
    if (res == LV_RES_OK) {

    }
    uint8_t header[56];
    lv_fs_read(&f, header, 56, NULL);
    memcpy(&f1_Header, header, 14);
    memcpy(&f1_Info, header+ 14, 40);
    lv_fs_close(&f);

    lv_obj_t* img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "C:/Users/pzh/Desktop/t3.bmp");
    lv_btn_create(lv_scr_act());

    while (!lv_win32_quit_signal)
    {
        lv_task_handler();
        Sleep(1);
    }

    return 0;
}