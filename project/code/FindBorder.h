/*
 * FindBorder.h
 *
 *  Created on: 2022年10月2日
 *      Author: SingThesong
 */
#include "zf_common_typedef.h"
#ifndef CODE_FINDBORDER_H_
#define CODE_FINDBORDER_H_



extern int LeftLine[ROW],RightLine[ROW];
extern int LeftLine1[ROW],RightLine1[ROW];
extern int Original_RightLine[ROW],Original_LeftLine[ROW];
extern int ValidRow;

void FindBorder(void);
void FindBorder1(void);


#endif /* CODE_FINDBORDER_H_ */
