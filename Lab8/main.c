/****************************************************************************
Author: Gabe DiMartino
Lab: PWM and DC Motor
Date Created: March 18, 2025
Last Modified: March 18, 2025
Description: DC motor lab
****************************************************************************/

#include "LCD.h"
#include "Keypad.h"
#include "delay.h"
#include "motorControl.h"

int main(void)
{
    LCD_init();
    LCD_Clear();
    LCD_data('0');  // Display initial speed as '0'
    MatrixKeypad_Init();
    Motor_Init(5000);

    while (1)
    {
        char input = MatrixKeypad_Scan();

        switch (input)
        {
            case 'E': // Reverse direction
                Set_Rev();
                break;
            case 'F': // Forward direction
                Set_Fwd();
                break;
            default:
                if (input >= '0' && input <= '9') // Ensure valid speed input
                {
                    Set_Speed(input - '0');
                }
                break;
        }
    }
}
