/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Implementation of Bar Graph LED interface functions.
****************************************************************************/

#include "BarGraphLED.h"

void BarGraphLED_Init(void) {
    SYSCTL->RCGCGPIO |= 0x20;
    while ((SYSCTL->PRGPIO & 0x20) == 0);
    GPIOF->DIR |= 0x1E;
    GPIOF->DEN |= 0x1E;
}

void BarGraphLED_Display(uint8_t value) {
    GPIOF->DATA = (GPIOF->DATA & ~0x1E) | ((value & 0x0F) << 1);
}
