/*
 * CommonFunction.c
 * Created on: 2024年10月11日
 * Author: 刘沐
 */

#include "zf_common_headfile.h"

//限幅
float LimitBoth_float(float a,float min,float max)
{
    if(a<min)
         return min;
    else if(a>max)
         return max;
    else return a;

}
int LimitBoth_int(int a,int min,int max)
{
    if(a<min)
         return min;
    else if(a>max)
         return max;
    else return a;

}
