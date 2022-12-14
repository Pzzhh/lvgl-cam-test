#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include "cam.h"

using namespace cv;
using namespace std;


Mat frame;
VideoCapture capture(0);    // 打开摄像头


lv_color_t cam_buf[700*700];
void cam_frame_buff_output(lv_color_t* buf, uint32_t size)
{
    uint32_t len = frame.rows * frame.cols, l = 0;
    lv_color_t b;
    //if (!frame.empty())	// 判断是否为空
    //{
    //    imshow("camera", frame);
    //}
    capture >> frame;    // 读取图像帧至frame
    //waitKey(0);
#if 1
    for (int i = 0; i < len; i++)
    {

        b.ch.red = (frame.data[i * 3 + 2] >> 3) & 0X1F;
        b.ch.green = (frame.data[i * 3 + 1] >> 2) & 0X3F;
        b.ch.blue = (frame.data[i * 3 + 0] >> 3) & 0X1F;
        cam_buf[i].full = b.full;
    }
    for (int y = 0; y < frame.rows; y++)
    {
        uint32_t py = y * 640;
        for (int x = 0; x < frame.cols; x++)
        {
            buf[py + x].full = cam_buf[l++].full;
        }

    }

#endif // 0
}
