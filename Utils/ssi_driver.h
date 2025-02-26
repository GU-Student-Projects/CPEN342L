/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: SSI header
****************************************************************************/

#ifndef SSI_DRIVER_H
#define SSI_DRIVER_H

#include "TM4C123GH6PM.h"
#include "delay.h"

void SSI_Init(void);
void SSI_Write(unsigned char data); 
unsigned char SSI_Read(void);

#endif


