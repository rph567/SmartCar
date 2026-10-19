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
    if(ControlRow < EndRow)
        ControlRow = EndRow + 1;
    else
        ControlRow = RowSet;
}

