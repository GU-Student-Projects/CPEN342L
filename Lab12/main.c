/****************************************************************************
Author: Gabe DiMartino
Lab: Stepper Motor Control
Date Created: April 22, 2025
Description: Main program to test stepper motor movement
****************************************************************************/

#include "stepper.h"
#include "delay.h"
#include "Keypad.h"
#include "LCD.h"

int main(void) {
    Stepper_Init();
    Keypad_Init();
    LCD_Init();

    uint8_t distance = 0;
    int8_t direction = 1;

    while (1) {
        LCD_Clear();
        LCD_Str("Enter distance:");

        char key =  MatrixKeypad_GetKey(); // waits for key 0-9
        if (key >= '0' && key <= '9') {
            distance = key - '0';

            LCD_Clear();
            LCD_Str("Dir: * = CW  # = CCW");

            char dirKey =  MatrixKeypad_GetKey(); // waits for * or #
            if (dirKey == '*') direction = 1;
            else if (dirKey == '#') direction = -1;
            else continue; // ignore invalid input

            LCD_Clear();
            LCD_Str("Moving ");
            LCD_data(direction == 1 ? '+' : '-');
            LCD_data(key); // Show distance

            Stepper_Seek(distance, 5, direction); // 5 ms per step delay
            delayMs(1000); // pause before next loop
        }
    }
}
