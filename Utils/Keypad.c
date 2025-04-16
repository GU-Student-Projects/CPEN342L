/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: April 16, 2025
Description: Implementation of matrix keypad interface functions (non-blocking)
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "Keypad.h"
#include "delay.h"

RowType ScanTab[5] = {
    { 0x01, "123A" },  // PD0
    { 0x02, "456B" },  // PD1
    { 0x04, "789C" },  // PD2
    { 0x08, "E0FD" },  // PD3
    { 0x00, "" }
};

static char lastKey = 0;
static uint8_t keyPressed = 0;

void MatrixKeypad_Init(void) {
    SYSCTL->RCGCGPIO |= 0x09;                  // Enable clocks for GPIOA and GPIOD
    while ((SYSCTL->PRGPIO & 0x09) != 0x09);   // Wait until ready

    // Set PA2-PA5 as input (columns) with pull-ups
    GPIOA->AFSEL &= ~0x3C;
    GPIOA->AMSEL &= ~0x3C;
    GPIOA->PCTL &= ~0x00FFFF00;
    GPIOA->DEN |= 0x3C;
    GPIOA->DIR &= ~0x3C;   // Inputs
    GPIOA->PUR |= 0x3C;    // Enable pull-ups

    // Set PD0–PD3 as digital outputs (rows)
    GPIOD->AFSEL &= ~0x0F;
    GPIOD->AMSEL &= ~0x0F;
    GPIOD->PCTL &= ~0x0000FFFF;
    GPIOD->DEN |= 0x0F;
    GPIOD->DIR &= ~0x0F;   // Initially inputs (set during scan)
}

char MatrixKeypad_Scan(void) {
    RowType *pt = ScanTab;
    uint32_t column;
    char key = 0;
    uint32_t j;

    while (pt->direction) {
        GPIOD->DIR = pt->direction;            // Make one row output
        GPIOD->DATA = ~pt->direction & 0x0F;   // Drive that row LOW, others HIGH (inactive)

        delayMs(5); // Let signals settle

        column = (GPIOA->DATA & 0x3C) >> 2;    // Read PA2–PA5

        for (j = 0; j < 4; j++) {
            if ((column & (1 << j)) == 0) {    // Key press pulls column LOW
                key = pt->keycode[j];

                delayMs(20); // debounce
                // Wait until key is released
                while ((GPIOA->DATA & 0x3C) != 0x3C);

                GPIOD->DIR &= ~0x0F; // Restore rows as inputs
                return key;
            }
        }

        pt++;
    }

    GPIOD->DIR &= ~0x0F; // Restore all rows to inputs after scan
    return 0;
}

char MatrixKeypad_GetKey(void) {
    char key = MatrixKeypad_Scan();

    if (key != 0 && keyPressed == 0) {
        lastKey = key;
        keyPressed = 1;
        return key;
    }

    if (key == 0) {
        keyPressed = 0;
    }

    return 0;
}
