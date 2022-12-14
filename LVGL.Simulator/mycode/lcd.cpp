#include <stdlib.h>
#include<stdint.h>
#include "extra_display.h"
#include "lcd.h"
#include "FONT.H"
#include "cam.h"
lv_color_t frema_buf[800 * 800];
typedef uint16_t u16;
typedef uint8_t u8;
typedef uint32_t u32;


static void LCD_Clear(uint16_t color, display_info * e)
{
    uint32_t px = e->w * e->h;
    for (int i = 0; i < px; i++)
    {
        frema_buf[i].full = color;
    }
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
static void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;
    uint32_t px=0;
    (sx < display1.w) ? 1 : sx = display1.w - 1;
    (sy < display1.h) ? 1 : sy = display1.h - 1;
    (ex < display1.w) ? 1 : ex = display1.w - 1;
    (ey < display1.h) ? 1 : ey = display1.h - 1;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        px = i * display1.w + sx;

        for (j = 0; j < xlen; j++)
        {
            display1.frema_buf[px+j].full = color;     //设置光标位置
        }
    }
}

static void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    uint32_t px = x + y * display1.w;
    frema_buf[px].full = color;
}

/**
    次底层
***/

//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
static void gui_draw_hline(u16 x0, u16 y0, u16 len, u16 color)
{
    if (len == 0)return;
    LCD_Fill(x0, y0, x0 + len - 1, y0, color);
}

uint16_t POINT_COLOR = 0X000;
uint16_t BACK_COLOR = 0Xffff;
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
static void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数	
    num = num - ' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t]; 	 	//调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];	//调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];	//调用2412字体
        else return;								//没有的字库
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            //if (y >= lcddev.height)return;		//超区域了
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                //if (x >= lcddev.width)return;	//超区域了
                break;
            }
        }
    }
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
static void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t* p)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        //if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
static void gui_fill_circle(u16 x0, u16 y0, u16 r, u16 color)
{
    u32 i;
    u32 imax = ((u32)r * 707) / 1000 + 1;
    u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
    u32 x = r;
    gui_draw_hline(x0 - r, y0, 2 * r, color);
    for (i = 1; i <= imax; i++)
    {
        if ((i * i + x * x) > sqmax)// draw lines from outside  
        {
            if (x > imax)
            {
                gui_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), color);
                gui_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), color);
            }
            x--;
        }
        // draw lines from inside (center)  
        gui_draw_hline(x0 - x, y0 + i, 2 * x, color);
        gui_draw_hline(x0 - x, y0 - i, 2 * x, color);
    }
}



static void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2, u8 size, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    if (x1 < size || x2 < size || y1 < size || y2 < size)return;
    delta_x = x2 - x1; //计算坐标增量 
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)incx = 1; //设置单步方向 
    else if (delta_x == 0)incx = 0;//垂直线 
    else { incx = -1; delta_x = -delta_x; }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;//水平线 
    else { incy = -1; delta_y = -delta_y; }
    if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴 
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)//画线输出 
    {
        gui_fill_circle(uRow, uCol, size, color);//画点 
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * 应用
*/


static void ctp_test(void)
{
    u8 t = 0;
    u8 i = 0;
    static u16 lastpos[5][2];		//最后一次的数据 

    if (display1.indev.x > 0 && display1.indev.x < display1.w)
    {
        //gui_fill_circle(display1.indev.x,display1.indev.y,5,RED);
        lcd_draw_bline(lastpos[t][0], lastpos[t][1], display1.indev.x, display1.indev.y, 2, RED);//画线
        lastpos[t][0] = display1.indev.x;
        lastpos[t][1] = display1.indev.y;
        //if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
        //{
        //    Load_Drow_Dialog();//清除
        //}
    }
    //else lastpos[t][0] = 0XFFFF;
}

/**
 * @brief lcd显示函数
*/
static void lcd_handle()
{
    static lv_color_t e;
    static int x;
   /* switch (x)
    {
    case 0:LCD_Clear(WHITE, &display1); break;
    case 1:LCD_Clear(BLACK, &display1); break;
    case 2:LCD_Clear(BLUE, &display1); break;
    case 3:LCD_Clear(RED, &display1); break;
    case 4:LCD_Clear(MAGENTA, &display1); break;
    case 5:LCD_Clear(GREEN, &display1); break;
    case 6:LCD_Clear(CYAN, &display1); break;
    case 7:LCD_Clear(YELLOW, &display1); break;
    case 8:LCD_Clear(BRRED, &display1); break;
    case 9:LCD_Clear(GRAY, &display1); break;
    case 10:LCD_Clear(LGRAY, &display1); break;
    case 11:LCD_Clear(BROWN, &display1); break;
    }*/
    /*LCD_Fill(0, 0, 200, 200, WHITE);
    LCD_Fill(100, 100, 400, 400, RED);*/
    //LCD_Clear()
    //POINT_COLOR = RED;
    ctp_test();
    
    LCD_ShowString(30, 40, 210, 24, 24, (uint8_t*)"Explorer STM32F4");
    LCD_ShowString(30, 70, 200, 16, 16, (uint8_t*)"TFTLCD TEST");
    LCD_ShowString(30, 90, 200, 16, 16, (uint8_t*)"ATOM@ALIENTEK");
    //LCD_ShowString(30, 110, 200, 16, 16, 0Xff);		//显示LCD ID	      					 
    LCD_ShowString(30, 130, 200, 12, 12, (uint8_t*)"2014/5/4");
    x++;
    if (x == 12)x = 0;
}


#define display_mode 2          //1为触摸测试 2为相机

/**
 * 初始化与服务函数
*/

void LCD_INIT()
{
#if display_mode==1
    display_set_size(400, 800, frema_buf);
#else
    display_set_size(640, 480, frema_buf);
#endif
    Extra_display_init(30);

}


/**
 * @brief 显示服务函数
 * @param e 定时器参数
*/
void Ex_display_refulsh(lv_timer_t* e)
{
    lv_obj_t* obj = (lv_obj_t*)e->user_data;
    display1.visible = lv_obj_is_visible(obj);          //获取当前图片状态
#if display_mode==1
    lcd_handle();
#else
    cam_frame_buff_output(frema_buf, 0);
#endif 
    lv_obj_invalidate(obj);                             //控件刷新
}




