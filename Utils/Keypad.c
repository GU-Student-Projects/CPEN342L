// keypad.c
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Implementation of matrix keypad interface functions
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "Keypad.h"
#include "delay.h"

RowType ScanTab[4] = {
    { 0x01, "123A" },
    { 0x02, "456B" },
    { 0x04, "789C" },
    { 0x08, "E0FD" }
};

void MatrixKeypad_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x12;   // Enable clock to GPIOB and GPIOE
    while ((SYSCTL_PRGPIO_R & 0x12) != 0x12) {};  // Wait until GPIOB and GPIOE are ready
    
    // Configure PORTE (Rows: PE3-PE0)
    GPIO_PORTE_AFSEL_R &= ~0x0F;    // Disable alternate functions on PE3-PE0
    GPIO_PORTE_AMSEL_R &= ~0x0F;    // Disable analog functionality on PE3-PE0
    GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // Clear pin control for PE3-PE0
    GPIO_PORTE_DEN_R |= 0x0F;       // Enable digital functionality on PE3-PE0
    GPIO_PORTE_DIR_R &= ~0x0F;      // Set PE3-PE0 as input (rows)
    GPIO_PORTE_PUR_R |= 0x0F;       // Enable pull-up resistors on PE3-PE0
    
    // Configure PORTB (Columns: PB3-PB0)
    GPIO_PORTB_AFSEL_R &= ~0x0F;    // Disable alternate functions on PB3-PB0
    GPIO_PORTB_AMSEL_R &= ~0x0F;    // Disable analog functionality on PB3-PB0
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // Clear pin control for PB3-PB0
    GPIO_PORTB_DATA_R &= ~0x0F;     // Clear PORTB data
    GPIO_PORTB_DEN_R |= 0x0F;       // Enable digital functionality on PB3-PB0
    GPIO_PORTB_DIR_R |= 0x0F;       // Set PB3-PB0 as output (columns)
    GPIO_PORTB_DR8R_R |= 0x0F;      // Drive 8mA on PB3-PB0 (columns)
}

char MatrixKeypad_Scan(int32_t *n) {
    int32_t row, col;
    int32_t count = 0;
    char key = 0;

    for (col = 0; col < 4; col++) {
        GPIO_PORTB_DATA_R = (1 << col);

        volatile uint32_t rows = GPIO_PORTE_DATA_R & 0x0F;

        for (row = 0; row < 4; row++) {
            if (!(rows & (1 << row))) {  
                count++;
                key = ScanTab[row].keycode[col];
            }
        }
    }
    
    *n = count;
    return key;
}

char MatrixKeypad_WaitPress(void) {
    char key, lastKey;
    int32_t n;
    
    do {
        lastKey = MatrixKeypad_Scan(&n);
        delayMs(50);
        key = MatrixKeypad_Scan(&n);
    } while (n != 1 || key != lastKey);
    
    return key;
}

void MatrixKeypad_WaitRelease(void) {
    int32_t n;
    char key1, key2;
    
    do {
        key1 = MatrixKeypad_Scan(&n);
        delayMs(50);
        key2 = MatrixKeypad_Scan(&n);
    } while (n != 0 || key1 != key2);
} 
