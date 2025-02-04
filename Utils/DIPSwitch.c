/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Implementation of DIP switch interface functions.
****************************************************************************/

#include "DIPSwitch.h"

void DIPSwitch_Init(void) {
    SYSCTL->RCGCGPIO |= 0x02; // Enable clock to GPIOB
    while ((SYSCTL->PRGPIO & 0x02) == 0);
    GPIOB->DIR &= ~0x0F; // Set PB0-PB3 as input
    GPIOB->DEN |= 0x0F;  // Enable digital function
}

uint8_t DIPSwitch_Read(void) {
    return GPIOB->DATA & 0x0F; // Read lower 4 bits of Port B
}
