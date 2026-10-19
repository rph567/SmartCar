/*
 * FindBorder.c
 *
 *  Created on: 2023年10月2日
 *      Author: 杨同舟
 *
 */

#include "zf_common_headfile.h"
int LeftLine[ROW] = {0},RightLine[ROW] = {0};
int Original_RightLine[ROW],Original_LeftLine[ROW];
int  LeftLine1[ROW],RightLine1[ROW];
int ValidRow=0;

//行扫描法找边界
void FindBorder(void)
{
  int i,j;

  for (i = ROW-1; i > ValidRow-1; i--)      //寻找边线，中线
  {
    for (j = Maxcolum; j > 1 ; j--)
    {
      if(videoData[i][j-2] == 0 && videoData[i][j-1] == 0 && videoData[i][j] == 0)
      {
        LeftLine[i] = j+1;
        break;
      }
      //else if(j == 2) LeftLine[i] = 0;
    }

    if(j==1)
    {
      for(;j>-1;j--)
      {
        if(videoData[i][j]==0)
        {
          LeftLine[i] = j+1;
          break;
        }
      }
    }
    if(j==-1) LeftLine[i] =0;

    for (j = Maxcolum; j < COL-2 ; j++)
    {
      if(videoData[i][j+2] == 0 && videoData[i][j+1] == 0 && videoData[i][j] == 0)
      {
        RightLine[i] = j-1;
        break;
      }
      //else if(j == COLUMN - 3) RightLine[i] = COLUMN -1;
    }

    if(j==COL-2)
    {
      for(;j<COL;j++)
      {
        if(videoData[i][j]==0)
        {
          RightLine[i] = j-1;
          break;
        }
      }
    }
    if(j==COL) RightLine[i] = COL-1;

    Original_LeftLine[i] = LeftLine[i];
    Original_RightLine[i] = RightLine[i];
  }
}
/************************************************************************/
/*
*函数名 void FindBorder1(void)
*函数功能：领域法找边界
*形参：无
*返回值：无
*日期：2017.09.02
*作者：范昌平
*备注：无
*/
/************************************************************************/

void FindBorder1(void)
{
  int i=0,j=0;
  /*****************************
  最近一行的边线
  *****************************/
  LeftLine1[ROW-1]=0;
  RightLine1[ROW-1]=COL-1;

  /***********************
  根据上一行边线位置寻线
  ***********************/
  for(i=ROW-2;i>-1;i--)
  {
    //上一个左边界的后一个点是黑点，则往右找
    if(videoData[i][LeftLine1[i+1]]==0)
    {
      for(j=LeftLine1[i+1]+1;j<COL-2;j++)
      {
        if(videoData[i][j+2] == 1 && videoData[i][j+1] == 1 && videoData[i][j] == 1)
        {
          LeftLine1[i]=j;
          break;
        }
      }
      if(j>=COL-2)
        LeftLine1[i]=COL/2;
    }
    //上一个左边界的后一个点是白点，则往左找
    else if(videoData[i][LeftLine1[i+1]]==1)
    {
      LeftLine1[i]=0;
      for(j=LeftLine1[i+1]-1;j>1;j--)
      {
        if(videoData[i][j-2] == 0 && videoData[i][j-1] == 0 && videoData[i][j] == 0)
        {
          LeftLine1[i]=j+1;
          break;
        }
      }
      if(j==1)
      {
        for(;j>-1;j--)
        {
          if(videoData[i][j]==0)
          {
            LeftLine1[i] = j+1;
            break;
          }
          LeftLine1[i] = 0;
        }
      }
    }
    //上一个右边界的后一个点是白点，则往右找
    if(videoData[i][RightLine1[i+1]]==1)
    {
      RightLine1[i]=COL-1;
      for(j=RightLine1[i+1]+1;j<COL-2;j++)
      {
        if(videoData[i][j+2] == 0 && videoData[i][j+1] == 0 && videoData[i][j] == 0)
        {
          RightLine1[i]=j;
          break;
        }
      }
      if(j==COL-2)
      {
        for(;j<COL;j++)
        {
          if(videoData[i][j]==0)
          {
            RightLine1[i] = j-1;
            break;
          }
          RightLine1[i] = COL-1;
        }
      }
    }
    //上一个右边界的后一个点是黑点，则往左找
    else if(videoData[i][RightLine1[i+1]]==0)
    {
      for(j=RightLine1[i+1]-1;j>1;j--)
      {
        if(videoData[i][j-2] == 1 && videoData[i][j-1] == 1 && videoData[i][j] == 1)
        {
          RightLine1[i]=j;
          break;
        }
      }
      if(j<=1)
      {
        RightLine1[i]=COL/2;
      }
    }
    if(LeftLine1[i]>=RightLine1[i])
    {
       ValidRow=i+1;
       break;
    }

  }
  if(i<=0)
  {
    ValidRow=0;
  }
}




