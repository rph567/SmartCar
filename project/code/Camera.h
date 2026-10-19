/*
* Camera.h
* Created on: 2024年10月11日
*/
#include "zf_common_typedef.h"
#ifndef CODE_CAMERA_H_
#define CODE_CAMERA_H_

#define ROW 100
#define COL 152
#define ROW_DIS 30
#define COL_DIS 28
#define BLACK 0
#define WHITE 1

extern int trans;

extern uint8 videoData[ROW][COL];  //图像存储数组
extern int16 dynamicBinary;        //图像动态二值化阈值
extern int16 staticBinary;         //图像固定二值化阈值
extern int LightPoint;

void Binary_Img(void); //二值化图像
uint8 Fast_OTSU(uint8 *img, int16 row, int16 col);  //大津法求阈值
void OLED_Print_Img128X64(uint8 img[ROW][COL]); //通过坐标变换将图像拉伸为128*64即oled屏幕大小进行显示

#endif /* CODE_CAMERA_H_ */
