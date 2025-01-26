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

#define RS 1
#define EN 2
#define LCD_LINE1 0x80
#define LCD_LINE2 0xC0

void LCD_init(void);
void PORTS_init(void);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_Str(const char *str);

#endif