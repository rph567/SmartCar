/*
 * FindControlRow.c
 * Created on: 2024??10??11??
 * Author: ????
 */

#include "zf_common_headfile.h"

int RowSet = 55;
int ControlRow=50;

void FindControlRow(void)
{
    int desiredRow = RowSet;

    if(EndRow + 2 > desiredRow)
    {
        desiredRow = EndRow + 2;
    }

    if(desiredRow >= ROW)
    {
        desiredRow = ROW - 1;
    }

    ControlRow = desiredRow;
}

