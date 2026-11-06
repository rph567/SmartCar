/*
 * Stop.c
 *
 * 首次斑马线作为发车标记，第二次识别后驶过再停车。
 */

#include "zf_common_headfile.h"

uint8 zebra_row_count = 0;
static uint8 zebra_confirm_count = 0;
static uint8 zebra_clear_count = 0;
static uint8 zebra_phase = 0;  /* 0:首次识别 1:驶离 2:等待一圈 3:驶过终点 */
static uint32 zebra_mark_pulses = 0;

#define ZEBRA_REARM_PULSES ((LEFT_PULSE_PER_METER + RIGHT_PULSE_PER_METER) * ZEBRA_REARM_CM / 200)
#define ZEBRA_PASS_PULSES  ((LEFT_PULSE_PER_METER + RIGHT_PULSE_PER_METER) * ZEBRA_PASS_CM / 200)

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

    /* 连续命中确认，避免同一条斑马线重复计圈 */
    if (zebra_rows >= ZEBRA_STOP_ROWS)
    {
        if (zebra_confirm_count < ZEBRA_CONFIRM_FRAMES)
            zebra_confirm_count++;
        zebra_clear_count = 0;
    }
    else
    {
        zebra_confirm_count = 0;
        if (zebra_clear_count < ZEBRA_CONFIRM_FRAMES)
            zebra_clear_count++;
    }

    if (zebra_phase == 0 && zebra_confirm_count >= ZEBRA_CONFIRM_FRAMES)
    {
        zebra_mark_pulses = travelPulseCount;
        zebra_phase = 1;
    }
    else if (zebra_phase == 1 &&
             zebra_clear_count >= ZEBRA_CONFIRM_FRAMES &&
             travelPulseCount - zebra_mark_pulses >= ZEBRA_REARM_PULSES)
    {
        zebra_phase = 2;
    }
    else if (zebra_phase == 2 && zebra_confirm_count >= ZEBRA_CONFIRM_FRAMES)
    {
        zebra_mark_pulses = travelPulseCount;
        zebra_phase = 3;
    }
    else if (zebra_phase == 3 &&
             travelPulseCount - zebra_mark_pulses >= ZEBRA_PASS_PULSES)
    {
        StopFlag = 1;
    }
}

