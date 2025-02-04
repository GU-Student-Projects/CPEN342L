/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Header file for Bar Graph LED interface functions.
****************************************************************************/

#ifndef BARGRAPHLED_H
#define BARGRAPHLED_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

void BarGraphLED_Init(void);
void BarGraphLED_Display(uint8_t value);

#endif
