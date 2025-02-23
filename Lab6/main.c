/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: DIP switch in LED bargraph out
****************************************************************************/

#include "ssi_driver.h"

int main(void) {
    uint8_t inputData;
    
    SSI_Init(); // Initialize SSI bus

    while (1) {
        inputData = SSI_Read_PortG(); // Read switch states
        SSI_Write_PortH(inputData);   // Output same value to LEDs
    }
}
