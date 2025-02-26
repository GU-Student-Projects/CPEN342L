/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: DIP switch in LED bargraph out
****************************************************************************/

#include "ssi_driver.h"

int main(void) {
    unsigned char switchData;

    SSI_Init();

    while (1) {
        switchData = SSI_Read(); // Read from 74HC165
        SSI_Write(switchData);   // Output to 74HC595
    }
}

