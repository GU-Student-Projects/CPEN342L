/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing
Date Created: February 2, 2025
Last Modified: February 3, 2025

Description: Header file for DIP switch interface functions.
****************************************************************************/

#ifndef DIPSWITCH_H
#define DIPSWITCH_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

void DIPSwitch_Init(void);
uint8_t DIPSwitch_Read(void);

#endif
