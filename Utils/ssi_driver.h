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
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define SSI0_BASE_ADDR      0x40008000  // Base address for SSI0
#define SSI_CLOCK_RATE      5000000     // 5 MHz baud rate

// Define Control Pins for 74HC165 and 74HC595
#define PORTG_LOAD_PIN      (1U << 3)   // PE3 for 74HC165 Load
#define PORTH_LATCH_PIN     (1U << 1)   // PD1 for 74HC595 Latch

void SSI_Init(void);
uint8_t SSI_Read_PortG(void);
void SSI_Write_PortH(uint8_t data);

#endif
