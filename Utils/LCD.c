// lcd.c
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Implementation of LCD control functions
****************************************************************************/

#include "LCD.h"
#include "delay.h"

void LCD_init(void) {
    PORTS_init();
    delayMs(20); // LCD controller reset sequence
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x20, 0); // use 4-bit data mode
    delayMs(1);
    LCD_command(0x28); // set 4-bit data, 2-line, 5x7 font
    LCD_command(0x06); // move cursor right
    LCD_command(0x01); // clear screen, move cursor to home
    LCD_command(0x0F); // turn on display, cursor blinking
}
/* PA2-PA5 for LCD D4-D7, respectively.
* PE0 for LCD R/S
* PC6 for LCD EN
*/
void PORTS_init(void) {
    SYSCTL_RCGCGPIO_R |= 0x1F;  // Enable clock to GPIOA, GPIOE, GPIOD, GPIOC

    // PORTA 5-2 LCD D7-D4
    GPIO_PORTA_AMSEL_R &= ~0x3C;      // Turn off analog functionality on PORTA 5-2
    GPIO_PORTA_DATA_R &= ~0x3C;       // Clear PORTA 5-2 (set them to low)
    GPIO_PORTA_DIR_R |= 0x3C;         // Set PORTA 5-2 as output
    GPIO_PORTA_DEN_R |= 0x3C;         // Enable digital functionality on PORTA 5-2

    // PORTE 0 for LCD R/S
    GPIO_PORTE_AMSEL_R &= ~0x01;      // Disable analog functionality on PE0
    GPIO_PORTE_DIR_R |= 0x01;         // Set PE0 as output for LCD RS
    GPIO_PORTE_DEN_R |= 0x01;         // Enable digital functionality on PE0
    GPIO_PORTE_DATA_R |= 0x01;        // Set PE0 idle high

    // PORTC 6 for LCD EN
    GPIO_PORTC_AMSEL_R &= ~0x40;      // Disable analog functionality on PC6
    GPIO_PORTC_DIR_R |= 0x40;         // Set PC6 as output for LCD EN
    GPIO_PORTC_DEN_R |= 0x40;         // Enable digital functionality on PC6
    GPIO_PORTC_DATA_R &= ~0x40;       // Set PC6 idle low

    // PORTD 7 for LCD CS
    GPIO_PORTD_AMSEL_R &= ~0x80;      // Disable analog functionality on PD7
    GPIO_PORTD_DIR_R |= 0x80;         // Set PD7 as output for LCD CS
    GPIO_PORTD_DEN_R |= 0x80;         // Enable digital functionality on PD7
    GPIO_PORTD_DATA_R |= 0x80;        // Set PD7 idle high
}



void LCD_nibble_write(char data, unsigned char control) {
    /* populate data bits */
    GPIO_PORTA_DIR_R |= 0x3C;         // Set PORTA 5-2 as GPIO output pins
    GPIO_PORTA_DATA_R &= ~0x3C;       // Clear data bits on PORTA 5-2
    GPIO_PORTA_DATA_R |= (data & 0xF0) >> 2;  // Set upper nibble (data bits)

    /* set R/S bit */
    if (control & RS)
        GPIO_PORTE_DATA_R |= 1;      // Set RS (PE0) high if control indicates RS
    else
        GPIO_PORTE_DATA_R &= ~1;     // Set RS (PE0) low if control indicates RS
    
    /* pulse E */
    GPIO_PORTC_DATA_R |= 1 << 6;     // Set the EN (PC6) high
    delayMs(0);                      // Delay for a small period
    GPIO_PORTC_DATA_R &= ~(1 << 6);  // Set the EN (PC6) low

    GPIO_PORTA_DIR_R &= ~0x3C;        // Set PORTA 5-2 as GPIO input pins (after write)
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0); // upper nibble first
    LCD_nibble_write(command << 4, 0); // then lower nibble
    if (command < 4)
    delayMs(2); // command 1 and 2 needs up to 1.64ms
    else
    delayMs(1); // all others 40 us
}
void LCD_data(char data) {
    LCD_nibble_write(data & 0xF0, RS); // upper nibble first
    LCD_nibble_write(data << 4, RS); // then lower nibble
    delayMs(1);
}

void LCD_Str(const char *str) {
    while(*str != '\0') {
        LCD_data(*str);
        str++;
    }
}