/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 29, 2025
Description: Implementation of matrix keypad interface functions
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "Keypad.h"
#include "delay.h"

RowType ScanTab[5] = {
    { 0x01, "123A" },
    { 0x02, "456B" },
    { 0x04, "789C" },
    { 0x08, "E0FD" },
    { 0x00, "" }
};

void MatrixKeypad_Init(void) {
    SYSCTL->RCGCGPIO |= 0x09;
    while ((SYSCTL->PRGPIO & 0x09) != 0x09);

    GPIOA->AFSEL &= ~0x3C;
    GPIOA->AMSEL &= ~0x3C;
    GPIOA->PCTL &= ~0x00FFFF00;
    GPIOA->DEN |= 0x3C;
    GPIOA->DIR &= ~0x3C;
    GPIOA->PUR |= 0x3C;

    GPIOD->AFSEL &= ~0x0F;
    GPIOD->AMSEL &= ~0x0F;
    GPIOD->PCTL &= ~0x0000FFFF;
    GPIOD->DEN |= 0x0F;
    GPIOD->DIR &= ~0x0F;
    GPIOD->DR8R |= 0x0F;
}

char MatrixKeypad_Scan(int32_t *Num) {
    RowType *pt = ScanTab;
    uint32_t column;
    char key = 0;
    uint32_t j;
    *Num = 0;

    while (pt->direction) {
        GPIOD->DIR = pt->direction;
        GPIOD->DATA &= ~0x0F;
        for (j = 1; j <= 10; j++);

        column = (GPIOA->DATA & 0x3C) >> 2;
        for (j = 0; j <= 3; j++) {
            if ((column & 0x01) == 0) {
                key = pt->keycode[j];
                (*Num)++;
            }
            column >>= 1;
        }
        pt++;
    }
    return key;
}

char MatrixKeypad_WaitPress(void) {
    char key, lastKey;
    int32_t n;

    do {
        lastKey = MatrixKeypad_Scan(&n);
        delayMs(20);
        key = MatrixKeypad_Scan(&n);
    } while (n != 1 || key != lastKey);

    return key;
}

void MatrixKeypad_WaitRelease(void) {
    int32_t n;
    char key1, key2;

    do {
        key1 = MatrixKeypad_Scan(&n);
        delayMs(20);
        key2 = MatrixKeypad_Scan(&n);
    } while (n != 0 || key1 != key2);
}
