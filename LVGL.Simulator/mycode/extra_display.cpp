#include "extra_display.h"

uint8_t flag;
static lv_res_t display_info(lv_img_decoder_t* decoder, const void* src, lv_img_header_t* header)
{
    LV_UNUSED(decoder);
    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/
    if (src_type == LV_IMG_SRC_FILE) {
        const char* fn = (const char*)src;
        if (strcmp(lv_fs_get_ext(fn), "my") == 0) {              /*Check the extension*/
            uint32_t w = 200;
            uint32_t h = 200;
            /*  memcpy(&w, headers + 18, 4);
              memcpy(&h, headers + 22, 4);*/
            header->w = w;
            header->h = h;
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
    static lv_color16_t t, * color_buff;
    color_buff = (lv_color16_t*)buf;
    for (int i = 0; i < len; i++)
    {
        color_buff[i].full = (t.full += 1);
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
    lv_img_decoder_set_info_cb(dec, display_info);
    lv_img_decoder_set_open_cb(dec, display_open);
    lv_img_decoder_set_read_line_cb(dec, display_read_line);
    lv_img_decoder_set_close_cb(dec, display_close);
}

void Ex_display_refulsh(lv_timer_t* e)
{
    lv_obj_t* obj = (lv_obj_t*)e->user_data;
    lv_obj_invalidate(obj);
}


void Extra_display_init(uint16_t fps)
{
    (fps <= 60 && fps >= 1) ? (fps = 1000 / fps) : (fps = 200);

    Extra_display_reg_init();
    lv_obj_t* img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "C:/1.my");
    lv_timer_create((lv_timer_cb_t)Ex_display_refulsh, fps, (void*)img);
}
