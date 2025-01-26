/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Header file for matrix keypad interface functions
****************************************************************************/

#ifndef KEYPAD_H
#define KEYPAD_H
#include <stdint.h>

struct Row {
    uint8_t direction;
    char keycode[4];
};
typedef const struct Row RowType;

RowType ScanTab[5] = {
    { 0x01, "123A" },
    { 0x02, "456B" },
    { 0x04, "789C" },
    { 0x08, "E0FD" },
    { 0x00, " " }
};

void MatrixKeypad_Init(void);
char MatrixKeypad_WaitPress(void);
void MatrixKeypad_WaitRelease(void);
char MatrixKeypad_Scan(int32_t *n);

#endif