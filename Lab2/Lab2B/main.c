// main.c
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 29, 2025
Description: This program interfaces with a 4x4 matrix keypad and 16x2 LCD.
Keys pressed are displayed on the LCD. At the end of the first row,
display moves to second row. When second row fills, display clears
after 1 second delay and returns to first row.
Inputs: 4x4 Matrix Keypad
Outputs: 16x2 LCD Display
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "LCD.h"
#include "Keypad.h"
#include "delay.h"

int main(void) {
    int col = 0;
    int row = 0;
    char key;
    
    PORTS_init();
    LCD_init();
    MatrixKeypad_Init();
    
    LCD_command(LCD_LINE1);
    
    while(1) {
        key = MatrixKeypad_WaitPress();
        MatrixKeypad_WaitRelease();

        LCD_data(key);
        col++;

        if(col >= 16) {
            if(row == 0) {
                LCD_command(LCD_LINE2);
                row = 1;
                col = 0;
            }
            else {
                delayMs(1000);          
                LCD_command(0x01);     
                delayMs(2);            
                
                LCD_command(LCD_LINE1);
                row = 0;
                col = 0;
            }
        }
    }
}
