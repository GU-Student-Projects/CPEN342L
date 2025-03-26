/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: March 25, 2025
Description: SSI implementation
****************************************************************************/

#include "ssi_driver.h"
#include "tm4c123gh6pm.h" // Include this if using Tiva C series

void SSI_Init(void) {
    SYSCTL->RCGCSSI |= 0x02;  // Enable clock for SSI1
    SYSCTL->RCGCGPIO |= 0x18; // Enable clock for Port D and E
    while ((SYSCTL->PRGPIO & 0x18) == 0) {}; // Wait for Port D, E to be ready

    // Configure Port D for SSI1 (PD0-PD3)
    GPIOD->AFSEL |= 0x0F;  // Enable alternate function on PD0-PD3
    GPIOD->PCTL = (GPIOD->PCTL & 0xFFFF0000) | 0x00002222; // Set SSI1
    GPIOD->AMSEL &= ~0x0F; // Disable analog function
    GPIOD->DEN |= 0x0F;    // Enable digital function

    // Disable SSI before configuration
    SSI1->CR1 = 0x00;

    // Configure clock and data format
    SSI1->CPSR = 4;  // Set clock prescaler (CPSDVSR = 4, must be even)
    SSI1->CR0 = (0x07 & 0x0F) | (4 << 8); // 8-bit data, SCR = 4

    // Enable SSI1
    SSI1->CR1 |= 0x02;

    // Configure PE3 as output for LOAD signal
    GPIOE->DIR |= 0x08;  
    GPIOE->DEN |= 0x08;  
    GPIOE->DATA |= 0x08; // Default state high
}

void SSI_Write(unsigned char data) {
    while ((SSI1->SR & 0x02) == 0) {}; // Wait until FIFO not full
    SSI1->DR = data; // Transmit a byte
    while ((SSI1->SR & 0x04) == 0) {}; // Wait until FIFO not empty
}

unsigned char SSI_Read(void) {
    GPIOE->DATA &= ~0x08; // Pull LOAD low to latch data
    __asm("NOP"); // Small delay instead of for loop
    GPIOE->DATA |= 0x08; // Back into shift mode

    while ((SSI1->SR & 0x01) == 0) {}; // Wait until FIFO has data
    SSI1->DR = 0x00; // Generate 8 SCK pulses to receive data
    while ((SSI1->SR & 0x04) == 0) {}; // Wait until FIFO not empty
    
    return SSI1->DR;
}
