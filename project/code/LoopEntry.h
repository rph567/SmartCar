/*
 * LoopEntry.h
 * 左侧环岛入口检测和短暂道路遮罩。
 * 只在摄像头帧循环调用，不进入2ms速度中断。
 */

#ifndef CODE_LOOPENTRY_H_
#define CODE_LOOPENTRY_H_

#include "zf_common_typedef.h"

#define LOOP_MASK_CONTROL_ROW 55

void LoopEntry_Update(void);
void LoopEntry_ApplyRightHalfMask(void);
void LoopEntry_ApplyBottomFifthMask(void);
uint8 LoopEntry_IsMaskActive(void);

#endif /* CODE_LOOPENTRY_H_ */