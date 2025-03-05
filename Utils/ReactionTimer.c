/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 4, 2025
Description: Implementation of reaction timer
****************************************************************************/

#include "ReactionTimer.h"

// Global variables for reaction timer
volatile uint32_t FirstCaptureTime = 0;  // Time of first button press
volatile uint32_t SecondCaptureTime = 0; // Time of second button press
volatile uint32_t ReactionPeriod = 0;    // Time difference between captures
volatile uint32_t OutputReady = 0;       // Flag to indicate measurement is ready
volatile uint8_t TimerState = 0;         // 0=ready, 1=waiting for second press, 2=completed

// Initialize Port F LEDs (PF1=Red, PF2=Blue, PF3=Green) and switches (PF0, PF4)
void PortF_Init(void) {
    SYSCTL->RCGCGPIO |= 0x20;            // Enable clock for Port F
    while ((SYSCTL->PRGPIO & 0x20) == 0) {}; // Wait for clock to stabilize
    
    GPIOF->LOCK = 0x4C4F434B;            // Unlock Port F
    GPIOF->CR |= 0x1F;                   // Allow changes to PF4-0
    
    // Configure Switches: PF0 and PF4 as inputs with pull-up
    GPIOF->DIR &= ~0x11;                 // PF0, PF4 as inputs
    GPIOF->PUR |= 0x11;                  // Enable pull-up resistors on PF0, PF4
    
    // Configure LEDs: PF1 (red), PF2 (blue), PF3 (green) as outputs
    GPIOF->DIR |= 0x0E;                  // PF3-1 as outputs
    GPIOF->DEN |= 0x1F;                  // Enable digital I/O on PF4-0
    
    // Ensure all LEDs are off initially
    GPIOF->DATA &= ~0x0E;
}

// Turn on red LED (PF1)
void LED_Red_On(void) {
    GPIOF->DATA |= 0x02;                 // Set PF1 high (red LED on)
}

// Turn off red LED (PF1)
void LED_Red_Off(void) {
    GPIOF->DATA &= ~0x02;                // Set PF1 low (red LED off)
}

// Read SW1 (PF4)
uint32_t Switch_Input1(void) {
    return (GPIOF->DATA & 0x10) ? 0 : 1; // Return 1 if pressed (active low)
}

// Read SW2 (PF0)
uint32_t Switch_Input2(void) {
    return (GPIOF->DATA & 0x01) ? 0 : 1; // Return 1 if pressed (active low)
}

// Initialize reaction timer (input capture)
void ReactionTimer_Init(void) {
    // Initialize state variables
    TimerState = 0;
    FirstCaptureTime = 0;
    SecondCaptureTime = 0;
    ReactionPeriod = 0;
    OutputReady = 0;
    
    // Initialize Port F for LEDs and switches
    PortF_Init();
    
    // Enable clocks for Timer0, Timer1, and Port B
    SYSCTL->RCGCTIMER |= 0x03;           // Enable Timer0 and Timer1
    SYSCTL->RCGCGPIO |= 0x02;            // Enable Port B clock
    while ((SYSCTL->PRGPIO & 0x02) == 0) {}; // Wait for clock to stabilize
    
    // Configure PB6 (T0CCP0) and PB4 (T1CCP0) for timer capture
    GPIOB->DIR &= ~0x50;                 // PB6,PB4 as inputs
    GPIOB->AFSEL |= 0x50;                // Enable alternate function
    GPIOB->DEN |= 0x50;                  // Enable digital I/O
    GPIOB->PCTL = (GPIOB->PCTL & 0xFF0F00FF) | 0x07070000; // Configure for Timer0A and Timer1A
    
    // Configure Timer0A (for first button - PB6)
    TIMER0->CTL &= ~0x01;                // Disable Timer0A during setup
    TIMER0->CFG = 0x04;                  // Configure for 16-bit mode
    TIMER0->TAMR = 0x07;                 // Configure for edge-time, capture mode
    TIMER0->CTL |= 0x0C;                 // Capture rising edge
    TIMER0->TAILR = 0xFFFF;              // Set maximum count for 16-bit timer
    TIMER0->TAPR = 0xFF;                 // Set prescaler to get slower time base
    TIMER0->IMR |= 0x04;                 // Enable capture interrupt
    TIMER0->ICR = 0x04;                  // Clear capture interrupt flag
    
    // Configure Timer1A (for second button - PB4)
    TIMER1->CTL &= ~0x01;                // Disable Timer1A during setup
    TIMER1->CFG = 0x04;                  // Configure for 16-bit mode
    TIMER1->TAMR = 0x07;                 // Configure for edge-time, capture mode
    TIMER1->CTL |= 0x0C;                 // Capture rising edge
    TIMER1->TAILR = 0xFFFF;              // Set maximum count for 16-bit timer
    TIMER1->TAPR = 0xFF;                 // Set prescaler to get slower time base
    TIMER1->IMR |= 0x04;                 // Enable capture interrupt
    TIMER1->ICR = 0x04;                  // Clear capture interrupt flag
    
    // Configure interrupts
    NVIC->IP[19] = 2 << 5;               // Priority 2 for Timer0A
    NVIC->IP[21] = 2 << 5;               // Priority 2 for Timer1A
    NVIC->ISER[0] = (1<<19) | (1<<21);   // Enable interrupts for Timer0A and Timer1A
    
    // Enable Timer0 (wait for first button press)
    TIMER0->CTL |= 0x01;                 // Enable Timer0A
    
    // Enable global interrupts
    __enable_irq();
}

// Timer0A interrupt handler (first button press)
void TIMER0A_Handler(void) {
    TIMER0->ICR = 0x04;                  // Clear capture interrupt flag
    
    // Only process if in ready state
    if (TimerState == 0) {
        // Capture first time
        FirstCaptureTime = TIMER0->TAR | ((uint32_t)TIMER0->TAPR << 16);
        
        // Update state to waiting for second press
        TimerState = 1;
        
        // Turn on red LED to indicate waiting for second button
        LED_Red_On();
        
        // Disable Timer0 and enable Timer1 for second capture
        TIMER0->CTL &= ~0x01;            // Disable Timer0A
        TIMER1->ICR = 0x04;              // Clear Timer1A interrupt flag
        TIMER1->CTL |= 0x01;             // Enable Timer1A
    }
}

// Timer1A interrupt handler (second button press)
void TIMER1A_Handler(void) {
    TIMER1->ICR = 0x04;                  // Clear capture interrupt flag
    
    // Only process if waiting for second press
    if (TimerState == 1) {
        // Capture second time
        SecondCaptureTime = TIMER1->TAR | ((uint32_t)TIMER1->TAPR << 16);
        
        // Calculate reaction time period
        uint32_t timeDiff;
        
        // Handle timer overflow
        if (SecondCaptureTime < FirstCaptureTime) {
            timeDiff = (0x01000000 - FirstCaptureTime) + SecondCaptureTime;
        } else {
            timeDiff = SecondCaptureTime - FirstCaptureTime;
        }
        
        // Store the period
        ReactionPeriod = timeDiff;
        
        // Update state to completed
        TimerState = 2;
        OutputReady = 1;
        
        // Turn off red LED
        LED_Red_Off();
        
        // Disable Timer1
        TIMER1->CTL &= ~0x01;            // Disable Timer1A
    }
}

// Get the reaction time in milliseconds
uint32_t ReactionTime_Get(void) {
    if (OutputReady) {
        OutputReady = 0;
        // Convert to milliseconds - 16MHz with 256 prescaler is 62500Hz timer
        // 1000 ms / 62500 Hz = 0.016 ms per tick
        return (ReactionPeriod * 16) / 1000;
    } else {
        return 0xFFFFFFFF;  // Return max value if not ready
    }
}

// Get the current state of the reaction timer
uint8_t ReactionTimer_Status(void) {
    return TimerState;
}

// Reset the reaction timer for a new measurement
void ReactionTimer_Reset(void) {
    // Reset all state variables
    TimerState = 0;
    FirstCaptureTime = 0;
    SecondCaptureTime = 0;
    ReactionPeriod = 0;
    OutputReady = 0;
    
    // Turn off red LED
    LED_Red_Off();
    
    // Reset and enable Timer0 for next measurement
    TIMER0->ICR = 0x04;                  // Clear Timer0A interrupt flag
    TIMER1->ICR = 0x04;                  // Clear Timer1A interrupt flag
    TIMER1->CTL &= ~0x01;                // Ensure Timer1A is disabled
    TIMER0->CTL |= 0x01;                 // Enable Timer0A for first capture
}
