/****************************************************************************
Author: Gabe DiMartino
Lab 1: GPIO and C Language
Date Created: January 21, 2025
Last Modified: January 21, 2025
Description: this program will display RGB LED according to the value read
from SW1 and SW2.
If both switches SW1 and SW2 are pressed, the LED should be blue
If just SW1 switch is pressed, the LED should be red
If just SW2 switch is pressed, the LED should be green
If neither SW1 or SW2 is pressed, the LED should be off
Inputs: SW1 and SW2
Outputs: RGB LED
****************************************************************************/

#include "TM4C123GH6PM.h"

// Dataclass to store the LED color values
#define RED_LED   0x02
#define BLUE_LED  0x04
#define GREEN_LED 0x08

void Delay(void){ // Delay the time by 0.1S
    unsigned long volatile time;
    time = 727240*200/91;
    while(time){
        time--;
    }
}

int main(void){
    SYSCTL_RCGCGPIO_R |= 0x20; //Initialize Clock and Port F
    while((SYSCTL_PRGPIO_R & 0x20) == 0){}
    
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_AMSEL_R = 0x00;
    GPIO_PORTF_PCTL_R = 0x00000000;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_AFSEL_R = 0x00;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;
    
    while(1){ // Infinite Loop to check if a switch is activateded
        unsigned char sw1 = (GPIO_PORTF_DATA_R & 0x10);
        unsigned char sw2 = (GPIO_PORTF_DATA_R & 0x01);
        
				// Reset LED
        GPIO_PORTF_DATA_R &= ~0x0E;
        
				// Output the color designated by the switch
        if (!sw1 && !sw2) {
            GPIO_PORTF_DATA_R |= BLUE_LED;
        }
        else if (!sw1 && sw2) {
            GPIO_PORTF_DATA_R |= RED_LED;
        }
        else if (sw1 && !sw2) {
            GPIO_PORTF_DATA_R |= GREEN_LED;
        }
        
				// Keep the color for 0.1 before reset
        Delay();
    }
}