/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: SSI header
****************************************************************************/

#ifndef SSI_DRIVER_H
#define SSI_DRIVER_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

#define PORTG_LOAD_PIN (1U << 3) // Load Pin PE3
#define PORTH_LATCH_PIN (1U << 1) // Latch Pin PD1
#define SSI0_CLK_PIN  (1U << 2)  // PA2
#define SSI0_MISO_PIN (1U << 3)  // PA3
#define SSI0_MOSI_PIN (1U << 5)  // PA5
#define SSI0_PINS     (SSI0_CLK_PIN | SSI0_MISO_PIN | SSI0_MOSI_PIN)

void SSI_Init(void);         // Initialize SSI and GPIOs
void SSI_Write(uint8_t data); // Write to 74HC595 (PortH)
uint8_t SSI_Read(void);      // Read from 74HC165 (PortG)

#endif // SSI_DRIVER_H


