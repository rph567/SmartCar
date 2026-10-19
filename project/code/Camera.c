/*
* Camera.c
* Created on: 2024年10月11日
*/
#include "zf_common_headfile.h"

uint8 videoData[ROW][COL];  //图像存储数组
int16 dynamicBinary;    //图像动态二值化阈值
int16 staticBinary = 0;   //图像固定二值化阈值

int LightPoint=20;//阈值补偿
int trans=4;   //图像裁剪左右偏移
/*
*函数名：Binary_Img
*功能：二值化图像，保存到videoData中，用于识别
*备注：采集图像
=========================================       ---
|                                        |        *
|                                        |        |
|--------+---------------------+----------        |
|        |                     |         |        |
|        |ROW    所用图像      |            |  MT9V03X_H
|        |                     |         |        |
|--------+---------------------+----------        |
|        |<--------COL-------->|         |        |
|                                        |        *
==========================================       ---
|<------------MT9V03X_W------------->|
*/
void Binary_Img(void)
{
    if(LightPoint >= 10000) LightPoint=-(LightPoint-10000);
    if (0 == staticBinary) //设置的静态阈值为0，就用软件动态二值化（适应性更强）
    {
        dynamicBinary = Fast_OTSU(mt9v03x_image[0], MT9V03X_H, MT9V03X_W)+LightPoint; //大津法计算图像二值化阈值
    }
    else                   //否则用固定阈值二值化（有时候固定阈值的效果更好）
    {
        dynamicBinary = staticBinary; //静态阈值
    }
    
    for (int16 row = 0, col; row < ROW; ++row) //去掉近处的行
    {
        for (col = COL_DIS / 2 -trans; col < COL + COL_DIS / 2 -trans; ++col) //去掉多余的行列
        {                                                       //灰度值小于阈值，认为是黑色（黑色灰度值=0，白色灰度值=255）
            videoData[row][col - COL_DIS / 2 + trans] = (mt9v03x_image[row][col] < dynamicBinary) ? BLACK : WHITE;
        }
    }
}

/*
*函数名：Fast_OTSU
*参数：image-图像数组,col-列数,row-行数
*返回值：图像二值化阈值
*功能：大津法计算二值化阈值（优化版）,参考 https://www.cnblogs.com/ranjiewen/p/6385564.html
*作者：朱俊宇 2018/11/26
*备注：经测试，优化前：180MHZ二值化188x120-33.8ms，152x100-32.4ms，
优化后：180MHZ二值化188x120- 3.0ms，152x100-2.2ms,128x64- 1.2ms
200MHZ二值化152x100- 1.8ms，220MHZ二值化152x100- 1.6ms
200MHZ二值化188x70- 1.35ms
*/
uint8 Fast_OTSU(uint8 *img, int16 row, int16 col)
{
    static int16 isFirstBinary = 1; //首次二值化标志位
    static int16 threshold; //二值化阈值
    int16 minGrayValue = 0, maxGrayValue = 255; //最小、最大有效灰度值（用来减少遍历）
    //int16 Pix_Dis = (ROW_DIS * MT9V03X_CSI_W) / 2;      //去除图像未参与识别的部分
    
    //以下变量要注意数据溢出，根据实际的分辨率修改数据类型。
    int grayHist[256] = {0}; //灰度直方图（这里至少要用int16，否则一种灰度出现的次数太多就会溢出）
    const int32 sumOfPixel = row * col; //像素总数
    int32 sumOfGrayValue = 0; //整幅图像的总灰度值
    int32 sumOfBackPixel = 0, sumOfForcePixel = 0; //前后像素总数
    int32 sumOfBackGray = 0, sumOfForceGray = 0; //前后总灰度值
    
    float w0, w1, u0, u1; //背景、前景像素比例
    float maxVariance = -1, tmpVariance; //类间方差
    
    for (int i = 0; i < sumOfPixel; ++i) //用一重循环可以避免计算数组下标时带入乘法，降低了复杂度
    {
        ++grayHist[img[i]]; //获得灰度直方图，统计0~255每个灰度出现了几次（这个算法大概90%的时间花在这里）
    }
    while (minGrayValue < 256 && 0 == grayHist[minGrayValue]) //搜索最小有效灰度值
    {
        ++minGrayValue;
    }
    while (maxGrayValue >= minGrayValue && 0 == grayHist[maxGrayValue]) //搜索最大有效灰度值
    {
        --maxGrayValue;
    }
    if (maxGrayValue - minGrayValue <= 1) //如果灰度值不超过2种(当然这种情况是几乎不可能的)
    {
        return minGrayValue; //直接算出最佳阈值
    }
    for (int16 i = minGrayValue ; i <= maxGrayValue; ++i)
    {
        sumOfGrayValue += i * grayHist[i]; //计算总灰度值
    }
    for (int16 i = minGrayValue; i <= maxGrayValue; ++i) //求方差的过程简化为一重循环，大大降低了算法复杂度
    {
        sumOfBackPixel += grayHist[i]; //背景总像素数
        sumOfBackGray += i * grayHist[i]; //背景总灰度值
        if (1 == isFirstBinary) //第一次二值化
        {
            goto GRAY_VARIANCE;
        }
        else //不是第一次二值化，本次阈值在上次阈值的附近（这样能快几十us）
        {
            if (i < threshold - 5 && maxGrayValue>threshold) //和上次阈值的差距 > 某个值，不计算类间方差。
            {
                continue;
            }
            if (i > threshold + 5 && minGrayValue<threshold)
            {
                return threshold; //最大类间方差对应的灰度值，认为是最佳分割阈值
            }
        }
    GRAY_VARIANCE: //计算类间方差
        sumOfForcePixel = sumOfPixel - sumOfBackPixel; //前景像素总数
        sumOfForceGray = sumOfGrayValue - sumOfBackGray; //前景总灰度值
        w0 = (float)sumOfBackPixel / sumOfPixel; //背景像素比例
        w1 = 1 - w0; //前景像素比例
        u0 = (float)sumOfBackGray / sumOfBackPixel; //背景灰度均值
        u1 = (float)sumOfForceGray / sumOfForcePixel; //前景灰度均值
        //代入u=w0*u0+w1*u1，类间方差w0*(u-u0)^2+w1*(u-u1)^2可化简为w0*w1*(u0-u1)^2
        tmpVariance = w0 * w1 *(u0 - u1) * (u0 - u1); //类间方差
        if (tmpVariance > maxVariance)
        {
            maxVariance = tmpVariance; //记录最大类间方差
            threshold = i; //记录最大类间方差对应的灰度值
        }
    }
    isFirstBinary = 0; //完成第一次二值化，清空标志位
    return threshold; //最大类间方差对应的灰度值，认为是最佳分割阈值
}

void OLED_Print_Img128X64(uint8 img[ROW][COL])
{
    uint8 tmp;
        OLED_CS(0);
    for (int16 i = 0; i < 8; ++i)
    {
        for (int16 j = 0; j < 128; ++j)
        {
            oled_set_coordinate(j, i);
            tmp = (img[(i * 8 + 0) * (ROW - 1) / 63][j * (COL - 1) / 127] << 0)
                     | (img[(i * 8 + 1) * (ROW - 1) / 63][j * (COL - 1) / 127] << 1)
                     | (img[(i * 8 + 2) * (ROW - 1) / 63][j * (COL - 1) / 127] << 2)
                     | (img[(i * 8 + 3) * (ROW - 1) / 63][j * (COL - 1) / 127] << 3)
                     | (img[(i * 8 + 4) * (ROW - 1) / 63][j * (COL - 1) / 127] << 4)
                     | (img[(i * 8 + 5) * (ROW - 1) / 63][j * (COL - 1) / 127] << 5)
                     | (img[(i * 8 + 6) * (ROW - 1) / 63][j * (COL - 1) / 127] << 6)
                     | (img[(i * 8 + 7) * (ROW - 1) / 63][j * (COL - 1) / 127] << 7);
            oled_write_data(tmp);
        }
    }
        OLED_CS(1);
}