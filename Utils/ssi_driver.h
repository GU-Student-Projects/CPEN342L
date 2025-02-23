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

// Define Load Pin for 74HC165 (connected to PE3)
#define PORTG_LOAD_PIN (1U << 3)
// Define Latch Pin for 74HC595 (connected to PD1)
#define PORTH_LATCH_PIN (1U << 1)

void SSI_Init(void);         // Initialize SSI and GPIOs
void SSI_Write(uint8_t data); // Write to 74HC595 (PortH)
uint8_t SSI_Read(void);      // Read from 74HC165 (PortG)

#endif // SSI_DRIVER_H


