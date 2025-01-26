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


void MatrixKeypad_Init(void) {
    SYSCTL->RCGCGPIO |= 0x09; 
    while((SYSCTL->PRGPIO & 0x09) != 0x09){}; 
    
    GPIOA->AFSEL &= ~0x3C; 
    GPIOA->AMSEL &= ~0x3C;
    GPIOA->PCTL  &= ~0x00FFFF00; 
    GPIOA->DEN   |= 0x3C; 
    GPIOA->DIR   &= ~0x3C;
    GPIOA->PUR   |= 0x3C;  // Enable pull-up resistors for PA5-PA2
    
    GPIOD->AFSEL &= ~0x0F; 
    GPIOD->AMSEL &= ~0x0F; 
    GPIOD->PCTL  &= ~0x0000FFFF; 
    GPIOD->DATA  &= ~0x0F; 
    GPIOD->DEN   |= 0x0F; 
    GPIOD->DIR   |= 0x0F;  // Set as output for scanning
    GPIOD->DR8R  |= 0x0F; 
}

char MatrixKeypad_Scan(int32_t *n) {
    int32_t row, col;
    int32_t count = 0;
    char key = 0;
    
    for(col = 0; col < 4; col++) {
        GPIOD->DATA = ~(1 << col) & 0x0F;
        uint32_t rows = (GPIOA->DATA & 0x3C) >> 2;
        
        for(row = 1; row < 5; row++) {
            if(!(rows & (1 << row))) {
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