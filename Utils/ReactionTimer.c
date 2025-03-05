/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 4, 2025
Description: Implementation of reaction timer
****************************************************************************/

#include "ReactionTimer.h"

// Global variables
volatile uint32_t FirstTime = 0;     // Time of first button press
volatile uint32_t SecondTime = 0;    // Time of second button press
volatile uint32_t ReactionTimeMs = 0;// Calculated reaction time in milliseconds
volatile uint8_t StatusFlag = 0;     // Status flag: 0=ready, 1=waiting for second press, 2=completed

// System clock frequency (16 MHz)
#define SYSCLK 16000000

void LED_Init(void) {
    // Enable clock for Port F
    SYSCTL->RCGCGPIO |= 0x20;
    while ((SYSCTL->PRGPIO & 0x20) == 0) {};  // Wait for clock to stabilize
    
    // Configure PF1 as output for red LED
    GPIOF->DIR |= 0x02;             // PF1 as output
    GPIOF->DEN |= 0x02;             // Enable digital I/O
}

void LED_On(void) {
    GPIOF->DATA |= 0x02;            // Turn on red LED
}

void LED_Off(void) {
    GPIOF->DATA &= ~0x02;           // Turn off red LED
}

void ReactionTimer_Init(void) {
    // Initialize status
    StatusFlag = 0;
    FirstTime = 0;
    SecondTime = 0;
    ReactionTimeMs = 0;
    
    // Enable clocks for Timer0, Timer1, and Port B
    SYSCTL->RCGCTIMER |= 0x03;      // Enable Timer0 and Timer1 clocks
    SYSCTL->RCGCGPIO |= 0x02;       // Enable Port B clock
    while ((SYSCTL->PRGPIO & 0x02) == 0) {};  // Wait for clock to stabilize
    
    // Configure PB6 (T0CCP0) and PB4 (T1CCP0) for timer capture
    GPIOB->DIR &= ~0x50;            // PB6 and PB4 as inputs
    GPIOB->AFSEL |= 0x50;           // Enable alternate function
    GPIOB->DEN |= 0x50;             // Enable digital I/O
    GPIOB->PCTL = (GPIOB->PCTL & 0xF0F0FFFF) | 0x07070000; // Configure for Timer0A and Timer1A
    
    // Configure Timer0A (for PB6 - first button)
    TIMER0->CTL &= ~0x01;           // Disable Timer0A during setup
    TIMER0->CFG = 0x04;             // Configure for 16-bit mode
    TIMER0->TAMR = 0x17;            // Configure for edge-time, capture mode, count up
    TIMER0->CTL |= 0x0C;            // Capture rising edge
    TIMER0->TAILR = 0xFFFF;         // Set maximum count for 16-bit timer
    TIMER0->TAPR = 0xFF;            // Set prescaler to get slower time base
    TIMER0->IMR |= 0x04;            // Enable capture interrupt
    TIMER0->ICR = 0x04;             // Clear capture interrupt flag
    
    // Configure Timer1A (for PB4 - second button)
    TIMER1->CTL &= ~0x01;           // Disable Timer1A during setup
    TIMER1->CFG = 0x04;             // Configure for 16-bit mode
    TIMER1->TAMR = 0x17;            // Configure for edge-time, capture mode, count up
    TIMER1->CTL |= 0x0C;            // Capture rising edge
    TIMER1->TAILR = 0xFFFF;         // Set maximum count for 16-bit timer
    TIMER1->TAPR = 0xFF;            // Set prescaler to get slower time base
    TIMER1->IMR |= 0x04;            // Enable capture interrupt
    TIMER1->ICR = 0x04;             // Clear capture interrupt flag
    
    // Configure interrupts
    NVIC->IP[19] = 2 << 5;          // Priority 2 for Timer0A
    NVIC->IP[21] = 2 << 5;          // Priority 2 for Timer1A
    NVIC->ISER[0] = (1 << 19) | (1 << 21); // Enable interrupts for Timer0A and Timer1A
    
    // Enable timers
    TIMER0->CTL |= 0x01;            // Enable Timer0A
    TIMER1->CTL |= 0x01;            // Enable Timer1A
}

void TIMER0A_Handler(void) {
    TIMER0->ICR = 0x04;            // Clear capture interrupt
    
    // Only process if in ready state
    if (StatusFlag == 0) {
        FirstTime = (TIMER0->TAR) | ((uint32_t)TIMER0->TAPR << 16); // Capture current time
        StatusFlag = 1;             // Change status to waiting for second press
        LED_On();                   // Turn on LED to indicate waiting for second button
    }
}

void TIMER1A_Handler(void) {
    TIMER1->ICR = 0x04;            // Clear capture interrupt
    
    // Only process if waiting for second press
    if (StatusFlag == 1) {
        SecondTime = (TIMER1->TAR) | ((uint32_t)TIMER1->TAPR << 16); // Capture current time
        
        // Calculate reaction time in milliseconds
        uint32_t timeDiff;
        // Handle timer overflow
        if (SecondTime < FirstTime) {
            timeDiff = (0x01000000 - FirstTime) + SecondTime;
        } else {
            timeDiff = SecondTime - FirstTime;
        }
        
        // Convert to milliseconds - 16MHz with 256 prescaler is 62500Hz timer
        // 1000 ms / 62500 Hz = 0.016 ms per tick
        ReactionTimeMs = (timeDiff * 16) / 1000;
        
        StatusFlag = 2;             // Change status to complete
        LED_Off();                  // Turn off LED
    }
}

uint32_t ReactionTime_Get(void) {
    return ReactionTimeMs;
}

uint8_t ReactionTime_Status(void) {
    return StatusFlag;
}

void ReactionTime_Reset(void) {
    StatusFlag = 0;
    FirstTime = 0;
    SecondTime = 0;
    ReactionTimeMs = 0;
    LED_Off();
}