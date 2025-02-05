/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Main program integrating keypad and LCD drivers to control
bar graph LED and display DIP switch value on LCD.

****************************************************************************/

#include "LCD.h"
#include "Keypad.h"
#include "DIPSwitch.h"
#include "BarGraphLED.h"
#include "delay.h"
#include <stdint.h>

int main(void) {
    uint8_t switchValue, prevSwitchValue = 0xFF;
    char key;

    LCD_init();
    DIPSwitch_Init();
    BarGraphLED_Init();
    MatrixKeypad_Init();

    while (1) {
        switchValue = DIPSwitch_Read();
        if (switchValue != prevSwitchValue) {
            LCD_command(LCD_LINE1);
            LCD_data(' ');
            LCD_data(' ');
            LCD_command(LCD_LINE1);
            LCD_data((switchValue / 10) + '0');
            LCD_data((switchValue % 10) + '0');
            prevSwitchValue = switchValue;
        }

        key = MatrixKeypad_GetKey();
        if (key != 0) {
            BarGraphLED_Display((uint8_t)key);
        }

        delayMs(10);
    }
    return 0;
}





