/*
 * FindMidline.c
 * Created on: 2024年10月11日
 * Author: 刘沐
 */

#include "zf_common_headfile.h"

int32 MidLine[ROW]={0};
int32 Error[ROW]={0};
int Original_MidLine[ROW]={0},Original_Error[ROW]={0};//根据路况修改Error后，可能还需要原来的Error用于计算或判断

void Find_MidLine(void)
{
    int i;
    for (i = ROW-1; i > RealEndRow; i--)        //寻找边线，中线
    {
          MidLine[i] = (RightLine[i] + LeftLine[i])/2;
          Original_MidLine[i]=MidLine[i];
          Error[i] = MidLine[i] - COL/2;
          Original_Error[i] = Error[i];
    }
}
