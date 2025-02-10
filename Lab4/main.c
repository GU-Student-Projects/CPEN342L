/****************************************************************************
Author: Gabe DiMartino
Lab: 3x5 LCD Interface
Date Created: February 9, 2025
Last Modified: February 9, 2025
Description: This program interfaces with a 3x5 LED. A DIP switch is activated
and the ASCII value is displayed.
Inputs: 1x4 DIP Switch
Outputs: 3x5 LED Display
****************************************************************************/

#include "DIPSwitch.h"
#include "MatrixDisplay.h"
#include "delay.h"
#include <stdint.h>

int main(void) {
    uint8_t switchValue, prevSwitchValue = 0xFF;
    
    // Initialize peripherals
    DIPSwitch_Init();
    GPIO_Init();

    while (1) {
        // Read DIP switch input
        switchValue = DIPSwitch_Read(); 

        // If switch value changed, update LED display
        if (switchValue != prevSwitchValue) {
            prevSwitchValue = switchValue;

            // Display hex digit on LED matrix
            display_hex_digit(switchValue);
        }

        delayMs(50);  // debounce
    }
    return 0;
}
