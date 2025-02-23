/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: SSI implementation
****************************************************************************/

#include "ssi_driver.h"

void SSI_Init(void) {
    // Enable SSI0 and GPIO Ports A, D, and E
    SYSCTL->RCGCSSI |= 0x01;  // Activate SSI0
    SYSCTL->RCGCGPIO |= (1U << 0) | (1U << 3) | (1U << 4);
    while ((SYSCTL->PRGPIO & ((1U << 0) | (1U << 3) | (1U << 4))) == 0) {}; // Wait for readiness

    // Configure SSI0 on PA2, PA3, PA5
    GPIOA->AFSEL |= SSI0_PINS;
    GPIOA->PCTL = (GPIOA->PCTL & 0xFF0F00FF) + 0x00202200;
    GPIOA->AMSEL &= ~SSI0_PINS;
    GPIOA->DEN |= SSI0_PINS;

    // Configure SSI as SPI Mode 0 (SPO = 0, SPH = 0)
    SSI0->CR1 = 0x00;  // Disable SSI, set as master
    SSI0->CPSR = 0x02; // Set clock divider (8MHz SSICLK from 16MHz system clock)
    SSI0->CR0 = (SSI0->CR0 & ~0x0F) + 0x07; // 8-bit data, Mode 0
    SSI0->CR1 |= 0x02; // Enable SSI0

    // Configure Port G (74HC165) - PE3 (Load)
    GPIOE->DIR |= PORTG_LOAD_PIN;
    GPIOE->DEN |= PORTG_LOAD_PIN;

    // Configure Port H (74HC595) - PD1 (Latch)
    GPIOD->DIR |= PORTH_LATCH_PIN;
    GPIOD->DEN |= PORTH_LATCH_PIN;
}


void SSI_Write(uint8_t data) {
    while ((SSI0->SR & 0x02) == 0) {}; // Wait until FIFO is not full
    SSI0->DR = data; // Transmit byte
    while (SSI0->SR & 0x10) {}; // Wait for transmission to complete

    // Latch data to 74HC595
    GPIOD->DATA |= PORTH_LATCH_PIN; // Pull PD1 HIGH
    for (volatile int i = 0; i < 10; i++); // Small delay
    GPIOD->DATA &= ~PORTH_LATCH_PIN; // Pull PD1 LOW
}

uint8_t SSI_Read(void) {
    // Load data from switches into 74HC165
    GPIOE->DATA &= ~PORTG_LOAD_PIN; // Pull PE3 LOW
    for (volatile int i = 0; i < 10; i++); // Small delay
    GPIOE->DATA |= PORTG_LOAD_PIN; // Pull PE3 HIGH to shift data

    // Read the data via SPI
    while ((SSI0->SR & 0x01) == 0) {}; // Wait until FIFO empty
    SSI0->DR = 0; // Send dummy data to clock in data
    while ((SSI0->SR & 0x04) == 0) {}; // Wait until FIFO is not empty
    return SSI0->DR;
}


