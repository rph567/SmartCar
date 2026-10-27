/*
 * LoopEntry.c
 *
 * 方案B：
 *   1. 在图像左侧三分之一的前视行带内逐列从下往上扫描；
 *   2. 取从下往上遇到的前两段白区，忽略更远的白区；
 *   3. 检查两段白区宽度和连续黑区列宽；
 *   4. 连续3帧确认后，强制接下来50帧只跟左侧路径；
 *   5. 50帧期间屏蔽右半屏和图像下方五分之一；
 *   6. 50帧结束后恢复完整图像。
 */

#include "zf_common_headfile.h"

#define LOOP_SCAN_TOP                   (ROW * 15 / 100) // 前视扫描上边界
#define LOOP_SCAN_BOTTOM                (ROW * 70 / 100) // 前视扫描下边界
#define LOOP_LEFT_THIRD                 (COL / 3)   // 只扫描左侧三分之一
#define LOOP_WHITE_WIDTH_MIN             3           // 白区纵向宽度至少3像素
#define LOOP_BLACK_GAP_MIN               8           // 两段白区之间黑区最小宽度
#define LOOP_BLACK_GAP_MAX               40          // 两段白区之间黑区最大宽度
#define LOOP_BLACK_REGION_MIN_COLUMNS    8           // 连续黑区至少8列
#define LOOP_CONFIRM_FRAMES              3          // 连续确认帧数
#define LOOP_MASK_DELAY_FRAMES            40         // 识别后延后几帧再开始遮罩
#define LOOP_MASK_FRAMES                 500         // 强制左侧循迹的图像帧数
#define LOOP_BOTTOM_MASK_START           (ROW * 4 / 5) // 下方五分之一起点

static uint8 confirm_frames = 0;   // 连续入口确认帧数
static uint8 delay_frames = 0;     // 开始遮罩前的剩余延迟帧数
static uint8 mask_frames = 0;      // 剩余强制遮罩帧数
static uint8 loop_done = 0;        // 本次上电只触发一次

/*
 * 检查一列中是否出现：
 *   下方白区 -> 黑色间隔 -> 上方白区
 * 从下往上只取前两段白区，忽略更远的上方白区。
 * 返回黑色间隔长度；无效时返回 -1。
 */
static int ColumnGapLength(int col)
{
    int row = LOOP_SCAN_BOTTOM - 1;
    int run_count = 0;
    int lower_start = 0;
    int lower_end = 0;
    int upper_start = 0;
    int upper_end = 0;

    while(row >= 0)
    {
        int start;
        int end;

        while((row >= LOOP_SCAN_TOP) && (videoData[row][col] == BLACK))
        {
            row--;
        }
        if(row < LOOP_SCAN_TOP)
        {
            break;
        }

        start = row;
        while((row >= LOOP_SCAN_TOP) && (videoData[row][col] == WHITE))
        {
            row--;
        }
        end = row + 1;

        if(run_count == 0)
        {
            lower_start = start;
            lower_end = end;
        }
        else if(run_count == 1)
        {
            upper_start = start;
            upper_end = end;
        }

        run_count++;
        if(run_count >= 2)
        {
            break;
        }
    }

    if(run_count < 2)
    {
        return -1;
    }

    {
        int lower_width = lower_start - lower_end + 1;
        int upper_width = upper_start - upper_end + 1;
        int gap = lower_end - upper_start - 1;

        if((lower_width < LOOP_WHITE_WIDTH_MIN) ||
           (upper_width < LOOP_WHITE_WIDTH_MIN))
        {
            return -1;
        }

        if((gap < LOOP_BLACK_GAP_MIN) || (gap > LOOP_BLACK_GAP_MAX))
        {
            return -1;
        }

        return gap;
    }
}

/* 统计左侧前视区域中连续黑区的列宽，不要求中心峰值 */
static uint8 FrameHasEntryPattern(void)
{
    int gaps[LOOP_LEFT_THIRD];
    int col;

    for(col = 0; col < LOOP_LEFT_THIRD; col++)
    {
        gaps[col] = ColumnGapLength(col);
    }

    col = 0;
    while(col < LOOP_LEFT_THIRD)
    {
        int start_col;
        int end_col;

        while((col < LOOP_LEFT_THIRD) && (gaps[col] < 0))
        {
            col++;
        }
        if(col >= LOOP_LEFT_THIRD)
        {
            break;
        }

        start_col = col;
        while((col < LOOP_LEFT_THIRD) && (gaps[col] >= 0))
        {
            col++;
        }
        end_col = col - 1;

        if((end_col - start_col + 1) >= LOOP_BLACK_REGION_MIN_COLUMNS)
        {
            return 1;
        }
    }

    return 0;
}
void LoopEntry_Update(void)
{
    if(loop_done)
    {
        /* 识别后先等待 LOOP_MASK_DELAY_FRAMES 帧，再开始遮罩 */
        if(delay_frames > 0)
        {
            delay_frames--;
            return;
        }

        /* 每帧入口处消耗一次遮罩帧，保证后续控制行修正使用本帧状态 */
        if(mask_frames > 0)
        {
            mask_frames--;
        }
        return;
    }

    /* 斑马线或出界停车期间不推进入口识别 */
    if(StopFlag)
    {
        confirm_frames = 0;
        return;
    }

    if(FrameHasEntryPattern())
    {
        confirm_frames++;
        if(confirm_frames >= LOOP_CONFIRM_FRAMES)
        {
            loop_done = 1;
            delay_frames = LOOP_MASK_DELAY_FRAMES;
            mask_frames = LOOP_MASK_FRAMES;
            confirm_frames = 0;
        }
    }
    else
    {
        confirm_frames = 0;
    }
}

/* 强制阶段屏蔽右半屏，必须先于 Find_EndRow 调用 */
void LoopEntry_ApplyRightHalfMask(void)
{
    int row;
    int col;

    if((delay_frames > 0) || (mask_frames == 0))
    {
        return;
    }

    for(row = 0; row < ROW; row++)
    {
        for(col = COL / 2; col < COL; col++)
        {
            videoData[row][col] = BLACK;
        }
    }
}

/* 强制阶段屏蔽下方五分之一，必须在 Find_EndRow 之后调用 */
void LoopEntry_ApplyBottomFifthMask(void)
{
    int row;
    int col;

    if((delay_frames > 0) || (mask_frames == 0))
    {
        return;
    }

    for(row = LOOP_BOTTOM_MASK_START; row < ROW; row++)
    {
        for(col = 0; col < COL; col++)
        {
            videoData[row][col] = BLACK;
        }
    }
}

uint8 LoopEntry_IsMaskActive(void)
{
    return ((delay_frames == 0) && (mask_frames > 0)) ? 1 : 0;
}