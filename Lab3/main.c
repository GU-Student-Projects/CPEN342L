/*
****************************************************************************

Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Main program integrating keypad and LCD drivers to control
bar graph LED and display DIP switch value on LCD.

****************************************************************************
*/

#include "LCD.h"
#include "Keypad.h"
#include "DIPSwitch.h"
#include "BarGraphLED.h"

int main(void) {
    int col = 0, row = 0;
    char key;
    uint8_t switchValue;

    // Initialize hardware components
    LCD_init();
    MatrixKeypad_Init();
    DIPSwitch_Init();
    BarGraphLED_Init();

    LCD_command(LCD_LINE1);
    LCD_Str("Switch Value:");

    while (1) {
        key = MatrixKeypad_WaitPress();
        MatrixKeypad_WaitRelease();

        BarGraphLED_Display((uint8_t)key);

        switchValue = DIPSwitch_Read();

        if (col >= 16) {
            if (row == 0) {
                LCD_command(LCD_LINE2);
                row = 1;
                col = 0;
            } else {
                delayMs(1000);
                LCD_command(0x01);
                delayMs(2);
                
                LCD_command(LCD_LINE1);
                LCD_Str("Switch Value:");
                row = 0;
                col = 0;
            }
        }

        LCD_command(LCD_LINE1 + 12);
        LCD_data((switchValue / 10) + '0');
        LCD_data((switchValue % 10) + '0');

        LCD_command(LCD_LINE1 + col);
        LCD_data(key);
        col++;
    }

    return 0;
}
