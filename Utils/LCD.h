// lcd.h
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Header file for LCD control functions
****************************************************************************/

#ifndef LCD_H
#define LCD_H

#include "TM4C123GH6PM.h"
#include <stdint.h>

#define RS 0x01
#define EN 2
#define HOME 0x02
#define LCD_LINE1 0x80
#define LCD_LINE2 0xC0

void PORTS_init(void);                  // Initialize GPIO ports for LCD
void LCD_init(void);                    // Initialize LCD
void LCD_nibble_write(char data, unsigned char control); // Write 4-bits to LCD
void LCD_command(unsigned char command); // Send command to LCD
void LCD_data(char data);               // Send data to LCD
void LCD_Str(const char *str);          // Display a string on LCD
void LCD_Clear(void);                   // Clear the LCD display
void LCD_Home(void);                    // Return cursor to home position
void LCD_SetCursor(unsigned char row, unsigned char col); // Set cursor position

#endif
