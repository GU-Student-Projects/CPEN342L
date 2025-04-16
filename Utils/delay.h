// delay.h
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Header file for delay function
****************************************************************************/

#ifndef DELAY_H
#define DELAY_H

#include "tm4c123gh6pm.h"
#include <stdint.h>

void delayMs(int n);
void delayUs(uint32_t us);

#endif