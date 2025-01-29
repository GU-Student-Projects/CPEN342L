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

RowType ScanTab[5] = {
    { 0x01, "123A" },
    { 0x02, "456B" },
    { 0x04, "789C" },
    { 0x08, "E0FD" },
    { 0x00, " " }
};

void MatrixKeypad_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x09;   // Enable clock to GPIOA and GPIOD
    while((SYSCTL_PRGPIO_R & 0x09) != 0x09) {};  // Wait until GPIOA and GPIOD are ready
    
    // Configure GPIOA (Rows)
    GPIO_PORTA_AFSEL_R &= ~0x3C;    // Disable alternate functions on PA5-PA2
    GPIO_PORTA_AMSEL_R &= ~0x3C;    // Disable analog functionality on PA5-PA2
    GPIO_PORTA_PCTL_R &= ~0x00FFFF00; // Clear pin control for PA5-PA2
    GPIO_PORTA_DEN_R |= 0x3C;       // Enable digital functionality on PA5-PA2
    GPIO_PORTA_DIR_R &= ~0x3C;      // Set PA5-PA2 as input (rows)
    GPIO_PORTA_PUR_R |= 0x3C;       // Enable pull-up resistors on PA5-PA2
    
    // Configure GPIOD (Columns)
    GPIO_PORTD_AFSEL_R &= ~0x0F;    // Disable alternate functions on PD3-PD0
    GPIO_PORTD_AMSEL_R &= ~0x0F;    // Disable analog functionality on PD3-PD0
    GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // Clear pin control for PD3-PD0
    GPIO_PORTD_DATA_R &= ~0x0F;     // Clear GPIOD data
    GPIO_PORTD_DEN_R |= 0x0F;       // Enable digital functionality on PD3-PD0
    GPIO_PORTD_DIR_R |= 0x0F;       // Set PD3-PD0 as output (columns)
    GPIO_PORTD_DR8R_R |= 0x0F;      // Drive 8mA on PD3-PD0 (columns)
}

char MatrixKeypad_Scan(int32_t *n) {
    int32_t row, col;
    int32_t count = 0;
    char key = 0;

    for(col = 0; col < 4; col++) {
        GPIO_PORTD_DATA_R = ~(1 << col) & 0x0F;  // Activate column by setting one low

        uint32_t rows = (GPIO_PORTA_DATA_R & 0x3C) >> 2;  // Read rows (PA5-PA2)

        for(row = 1; row < 5; row++) {  // Scan rows
            if(!(rows & (1 << row))) {
                count++;
                key = ScanTab[row].keycode[col];  // Get key from ScanTab
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
        delayMs(20);  // 20ms debounce
        key = MatrixKeypad_Scan(&n);
    } while(n != 1 || key != lastKey);
    
    return key;
}

void MatrixKeypad_WaitRelease(void) {
    int32_t n;
    char key1, key2;
    
    do {
        key1 = MatrixKeypad_Scan(&n);
        delayMs(20);  // 20ms debounce
        key2 = MatrixKeypad_Scan(&n);
    } while(n != 0 || key1 != key2);
}
