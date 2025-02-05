/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: February 3, 2025
Description: Header file for matrix keypad interface functions (non-blocking)
****************************************************************************/

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

struct Row {
    uint8_t direction;
    char keycode[4];
};
typedef const struct Row RowType;

extern RowType ScanTab[5];

void MatrixKeypad_Init(void);
char MatrixKeypad_GetKey(void);
char MatrixKeypad_Scan(void);

#endif
