// main.c
/****************************************************************************
Author: Gabe DiMartino
Lab: LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: This program interfaces with a 16x2 LCD.
A string is read and displayed on the LCD. At the end of the first string,
display moves to second row.
Inputs: None
Outputs: 16x2 LCD Display
****************************************************************************/

#include "LCD.h"

int main(void) {
    LCD_init();
    
    LCD_command(LCD_LINE1);
    LCD_Str("CPEN342L");
    
    LCD_command(LCD_LINE2);
    LCD_Str("Gabe DiMartino");
    
    while(1) {
    }
}