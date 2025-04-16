/**************************************************************************** 
Author: Gabe DiMartino 
Lab: Digital Signal Analyzer 
Date Created: April 15, 2025 
Last Modified: April 15, 2025 
Description: Implementation of DSA

****************************************************************************/  

#include "TM4C123GH6PM.h"

void ADC1_InitCh8(void) {
    volatile uint32_t delay;

    SYSCTL->RCGCADC |= 0x02;    // Activate ADC1 clock
    SYSCTL->RCGCGPIO |= 0x10;   // Activate Port E clock
    delay = SYSCTL->RCGCGPIO;   // Wait for stabilization

    GPIOE->DIR &= ~0x20;        // PE5 as input
    GPIOE->AFSEL |= 0x20;       // Enable alternate function on PE5
    GPIOE->DEN &= ~0x20;        // Disable digital I/O
    GPIOE->AMSEL |= 0x20;       // Enable analog mode on PE5

    ADC1->PC = 0x05;            // 500K samples/sec
    ADC1->SSPRI = 0x3210;       // Sequencer 3 lowest priority
    ADC1->ACTSS &= ~0x08;       // Disable SS3 during setup
    ADC1->EMUX &= ~0xF000;      // Software trigger
    ADC1->SSMUX3 = 8;           // Set to AIN8 (PE5)
    ADC1->SSCTL3 = 0x06;        // IE0 + END0
    ADC1->IM &= ~0x08;          // Disable SS3 interrupt
    ADC1->ACTSS |= 0x08;        // Enable SS3
}

uint16_t ADC1_InCh8(void) {
    ADC1->PSSI = 0x08;               // Start conversion
    while ((ADC1->RIS & 0x08) == 0); // Wait for completion
    uint16_t result = ADC1->SSFIFO3 & 0xFFF;  // Read 12-bit result
    ADC1->ISC = 0x08;                // Clear completion flag
    return result;
}