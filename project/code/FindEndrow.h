/*
 * FindEndrow.h
 * Created on: 2024年10月11日
 * Author: 刘沐
 */
#include "zf_common_typedef.h"
#ifndef CODE_FINDENDROW_H_
#define CODE_FINDENDROW_H_



extern int16 Maxcolum,EndRow;
extern int RealEndRow,CorrectedEndRow;
extern int StopFlag;

void Find_EndRow();
void CorrectEndRow(void);

#endif /* CODE_FINDENDROW_H_ */
