#include "extra_display.h"

struct display_info display1;

uint8_t flag;

/*
*   驱动区
*/
static lv_res_t display_get_info(lv_img_decoder_t* decoder, const void* src, lv_img_header_t* header);
//图片信息读取
static lv_res_t display_open(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc);
//图片打开（用来确认图片类型）
static lv_res_t display_read_line(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc,
    lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t* buf);
//读取图片信息
static void display_close(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc);
//关闭读取(空函数)
static lv_res_t display_get_info(lv_img_decoder_t* decoder, const void* src, lv_img_header_t* header)
{
    LV_UNUSED(decoder);
    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/
    if (src_type == LV_IMG_SRC_FILE) {
        const char* fn = (const char*)src;
        if (strcmp(lv_fs_get_ext(fn), "my") == 0) {              /*Check the extension*/
            /*  memcpy(&w, headers + 18, 4);
              memcpy(&h, headers + 22, 4);*/
            header->w = display1.w;
            header->h = display1.h;
            header->always_zero = 0;
            flag = 1;
            header->cf = LV_IMG_CF_TRUE_COLOR;
            return LV_RES_OK;
        }
    }
    return LV_RES_INV;
}

static lv_res_t display_read_line(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc,
    lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t* buf)
{
    LV_UNUSED(decoder);
    //bmp_dsc_t * b = dsc->user_data;

    lv_color_t* color_buff = (lv_color_t*)buf;
    uint32_t px = x + y * display1.w;
    for (int i = 0; i < len; i++)
    {
        color_buff[i] = display1.frema_buf[i + px];
    }
    return LV_RES_OK;

}
static lv_res_t display_open(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc)
{
    LV_UNUSED(decoder);
    if (dsc->src_type == LV_IMG_SRC_FILE) {
        const char* fn = (const char*)dsc->src;

        if (strcmp(lv_fs_get_ext(fn), "my") != 0) {
            return LV_RES_INV;       /*Check the extension*/
        }
        else
        {
            return LV_RES_OK;

        }
    }
    return LV_RES_INV;
}

static void display_close(lv_img_decoder_t* decoder, lv_img_decoder_dsc_t* dsc)
{
    LV_UNUSED(decoder);

}

void Extra_display_reg_init()
{

    lv_img_decoder_t* dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, display_get_info);
    lv_img_decoder_set_open_cb(dec, display_open);
    lv_img_decoder_set_read_line_cb(dec, display_read_line);
    lv_img_decoder_set_close_cb(dec, display_close);
}

/**
** 附件驱动区
*/

/**
 * @brief 触摸驱动
 * @param e
*/

void touch_handle(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_point_t click_pos;
    lv_area_t click_area;
    if (code < LV_EVENT_CLICKED) {
        lv_obj_t* obj = lv_event_get_target(e);
        lv_indev_get_point(lv_indev_get_act(), &click_pos);         //读取当前触摸点（零点为屏幕左上角）
        lv_obj_get_click_area(obj, &click_area);                    //读取图片的显示范围（零点为屏幕左上角）
        display1.indev.x = click_pos.x - click_area.x1;             //获得触摸的位置（零点在控件的左上角）
        display1.indev.y = click_pos.y - click_area.y1;             //获得触摸的位置（零点在控件的左上角）    
        (display1.indev.x < 0) ? display1.indev.x = 0 : 1;          //控制边界防止超出边界
        (display1.indev.y < 0) ? display1.indev.y = 0 : 1;
        (display1.indev.x > click_area.x2 - click_area.x1) ? display1.indev.x = click_area.x2 - click_area.x1 : 1;
        (display1.indev.y > click_area.y2 - click_area.y1) ? display1.indev.y = click_area.y2 - click_area.y1 : 1;
    }
    else
    {
        if (code == LV_EVENT_CLICKED)
        {
            display1.indev.x = 0XFFFF;
            display1.indev.y = 0XFFFF;
            //LV_LOG_USER("\r\nreleased ");
        }
    }
}

/**
 * api区
 *
*/

void Extra_display_init(uint16_t fps)
{
    (fps <= 60 && fps >= 1) ? (fps = 1000 / fps) : (fps = 200);

    lv_obj_t* tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    Extra_display_reg_init();
    lv_obj_t* img = lv_img_create(tab1);
    img->user_data = (void*)&display1;
    lv_img_set_src(img, "C:/1.my");
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_timer_create((lv_timer_cb_t)Ex_display_refulsh, fps, (void*)img);
    lv_obj_add_event_cb(img, touch_handle, LV_EVENT_ALL, 0);
    lv_obj_set_style_transform_zoom(img, 250, 0);
}

void display_set_size(uint32_t w, uint32_t h, lv_color_t* e)
{
    display1.w = w;
    display1.h = h;
    display1.frema_buf = e;
}

