/*
 * Stop.h
 *
 * 斑马线停车检测。
 */

#ifndef CODE_STOP_H_
#define CODE_STOP_H_

#include "zf_common_typedef.h"

/* 一行内至少出现多少次黑白跳变，才认为这一行像斑马线 */
#define ZEBRA_TRANS_MIN         8
/* 一行内至少有多少个黑点，避免把少量噪声当成斑马线 */
#define ZEBRA_BLACK_MIN         6
/* 一行内至少有多少个白点，避免整行偏黑 */
#define ZEBRA_WHITE_MIN         10
/* 连续多少行满足条件后停车：大于 3 行，即至少 4 行 */
#define ZEBRA_STOP_ROWS         4

/* 扫描区域：图像中间左右各去掉 20 列 */
#define ZEBRA_COL_MARGIN        20

/* 只扫描图像最底部若干行，数值越小，停车越晚 */
#define ZEBRA_SCAN_ROWS         20

extern uint8 zebra_row_count;

void Zebra_Stop_Detect(void);

#endif /* CODE_STOP_H_ */
