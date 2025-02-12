/****************************************************************************
Author: Gabe DiMartino
Lab: 3x5 LCD Interface
Date Created: February 9, 2025
Last Modified: February 9, 2025
Description: 3x5 LED matrix implementation
****************************************************************************/

#include "delay.h"
#include "MatrixDisplay.h"
#include "tm4c123gh6pm.h"

// LED matrix patterns for hex digits 0-F
unsigned char num[16][5] = {
    {0xF7, 0xED, 0xDD, 0xBD, 0x7F}, // '0'
    {0xF1, 0xE9, 0xD9, 0xB9, 0x79}, // '1'
    {0xF7, 0xE9, 0xDF, 0xBC, 0x7F}, // '2'
    {0xF7, 0xE9, 0xDF, 0xB9, 0x7F}, // '3'
    {0xF5, 0xED, 0xDF, 0xB9, 0x79}, // '4'
    {0xF7, 0xEC, 0xDF, 0xB9, 0x7F}, // '5'
    {0xF7, 0xEC, 0xDF, 0xBD, 0x7F}, // '6'
    {0xF7, 0xE9, 0xD9, 0xB9, 0x79}, // '7'
    {0xF7, 0xED, 0xDF, 0xBD, 0x7F}, // '8'
    {0xF7, 0xED, 0xDF, 0xB9, 0x7F}, // '9'
    {0xF7, 0xED, 0xDF, 0xBD, 0x7D}, // 'A'
    {0xF6, 0xED, 0xDE, 0xBD, 0x7E}, // 'B'
    {0xF7, 0xEC, 0xDC, 0xBC, 0x7F}, // 'C'
    {0xF6, 0xED, 0xDD, 0xBD, 0x7E}, // 'D'
    {0xF7, 0xEC, 0xDF, 0xBC, 0x7F}, // 'E'
    {0xF7, 0xEC, 0xDF, 0xBC, 0x7C}  // 'F'
};

// Function to initialize GPIO for Port D and Port F
void GPIO_Init() {
    SYSCTL->RCGCGPIO |= 0x28;  // Enable clock for Port D and Port F
    while ((SYSCTL->PRGPIO & 0x28) == 0); // Wait until ready

    // Configure Port D (PD0-PD3 as outputs)
    GPIOD->DIR |= 0x0F;
    GPIOD->DEN |= 0x0F;

    // Configure Port F (PF1-PF4 as outputs)
    GPIOF->DIR |= 0x1E;
    GPIOF->DEN |= 0x1E;
}

void display_hex_digit(unsigned char digit) {
    if (digit > 15) return;  // Only accept 0-F

    for (int i = 0; i < 5; i++) {
        unsigned char nNum = ~num[digit][i];  // Invert bits for LED matrix

        GPIOD->DATA = nNum;                 // Send column data (PD0-PD3)
        GPIOF->DATA = ((nNum & 0xF0) >> 3);  // Send row data (PF1-PF4)

        delayMs(1);  // Short delay to stabilize display
    }
}
