/*
 * Stop.c
 *
 * 基础斑马线检测：
 * 在图像中间区域逐行统计黑白跳变次数。
 * 如果同一帧中有超过 3 行满足条件，就直接置位 StopFlag。
 */

#include "zf_common_headfile.h"

uint8 zebra_row_count = 0;

void Zebra_Stop_Detect(void)
{
    int16 row;
    int16 col;
    uint8 zebra_rows = 0;

    if (StopFlag)
    {
        return;
    }

    /*
     * 只扫描图像最底部的 ZEBRA_SCAN_ROWS 行。
     * 从 row=99 开始往上扫描，斑马线进入底部区域后才停车。
     */
    for (row = ROW - 1;
         row >= ROW - ZEBRA_SCAN_ROWS;
         row--)
    {
        uint8 trans_cnt = 0;
        uint8 black_cnt = 0;
        uint8 white_cnt = 0;

        /*
         * 只看图像中间区域，减少左右边缘和背景的干扰。
         * videoData 中 BLACK=0，WHITE=1。
         */
        for (col = ZEBRA_COL_MARGIN;
             col < COL - ZEBRA_COL_MARGIN;
             col++)
        {
            if (videoData[row][col] != videoData[row][col - 1])
            {
                trans_cnt++;
            }

            if (videoData[row][col] == BLACK)
            {
                black_cnt++;
            }
            else
            {
                white_cnt++;
            }
        }

        if ((trans_cnt >= ZEBRA_TRANS_MIN) &&
            (black_cnt >= ZEBRA_BLACK_MIN) &&
            (white_cnt >= ZEBRA_WHITE_MIN))
        {
            zebra_rows++;
        }
    }

    zebra_row_count = zebra_rows;

    /* 大于 3 行像斑马线，直接停车 */
    if (zebra_rows >= ZEBRA_STOP_ROWS)
    {
        StopFlag = 1;
    }
}

