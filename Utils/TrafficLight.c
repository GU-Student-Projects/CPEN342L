/****************************************************************************
Author: Gabe DiMartino
Lab: Periodic Interrupt-Driven Traffic Lights
Date Created: February 16, 2025
Last Modified: February 16, 2025

Description: Implementation of Traffic Light LED interface functions.
****************************************************************************/

#include "TrafficLight.h"

// State durations
static const uint32_t STATE_DELAYS[] = {10, 2, 1, 10, 2, 1};
static volatile uint32_t SysTick_Count = 0;
static volatile TrafficState currentState = NS_GREEN_EW_RED;

void PLL_Init(void) {
    // 1. Enable USERCC2 in RCC2
    SYSCTL->RCC2 |= 0x80000000;  // Use RCC2
    
    // 2. Bypass PLL while initializing
    SYSCTL->RCC2 |= 0x00000800;  // BYPASS2, PLL bypass
    
    // 3. Select the crystal value and oscillator source
    SYSCTL->RCC = (SYSCTL->RCC & ~0x000007C0) + 0x00000540;  // Clear bits 10-6, then configure for 16 MHz crystal
    SYSCTL->RCC2 &= ~0x00000070;  // Configure for main oscillator source
    
    // 4. Activate PLL by clearing PWRDN
    SYSCTL->RCC2 &= ~0x00002000;
    
    // 5. Set the desired system divider and the SYSDIV2LSB bit
    SYSCTL->RCC2 |= 0x40000000;   // Use 400 MHz PLL
    // Set system clock divisor for 10MHz (400MHz / 40 = 10MHz)
    SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (39 << 22);
    
    // 6. Wait for the PLL to lock by polling PLLLRIS
    while((SYSCTL->RIS & 0x00000040) == 0) {};
    
    // 7. Enable PLL by clearing BYPASS
    SYSCTL->RCC2 &= ~0x00000800;
}

// Initialize Port B for LED control
static void PortB_Init(void) {
    SYSCTL->RCGCGPIO |= 0x02;                // Enable clock to Port B
    while((SYSCTL->PRGPIO & 0x02) == 0) {};  // Wait
    GPIO_PORTB->DIR |= 0x3F;                 // Set PB5-0 as outputs
    GPIO_PORTB->DEN |= 0x3F;                 // Enable digital I/O on PB5-0
    GPIO_PORTB->DATA = 0;                    // Initialize all LEDs to off
}

// Initialize SysTick for periodic interrupts
static void SysTick_Init(uint32_t period) {
    NVIC_ST_CTRL_R = 0;              // Disable SysTick during configuration
    NVIC_ST_RELOAD_R = period - 1;   // Set reload value
    NVIC_ST_CURRENT_R = 0;           // Clear current value
    NVIC_ST_CTRL_R = 0x07;           // Enable SysTick with interrupts
}

// Set traffic light state
static void SetTrafficLights(TrafficState state) {
    uint32_t light_pattern = 0;
    
    switch(state) {
        case NS_GREEN_EW_RED:
            light_pattern = NS_GRN | EW_RED;
            break;
        case NS_YEL_EW_RED:
            light_pattern = NS_YEL | EW_RED;
            break;
        case NS_RED_EW_RED_1:
        case NS_RED_EW_RED_2:
            light_pattern = NS_RED | EW_RED;
            break;
        case NS_RED_EW_GREEN:
            light_pattern = NS_RED | EW_GRN;
            break;
        case NS_RED_EW_YEL:
            light_pattern = NS_RED | EW_YEL;
            break;
    }
    GPIO_PORTB->DATA = light_pattern;
}

void TrafficLight_Init(void) {
    PortB_Init();
    
    // Initialize SysTick for 1-second intervals (10MHz clock)
    SysTick_Init(10000000);
    
    SetTrafficLights(currentState);
}

void TrafficLight_Update(void) {
    SetTrafficLights(currentState);
    delayMs(STATE_DELAYS[currentState]*1000);
    currentState = (currentState + 1) % 6;
}

void SysTick_Handler(void) {
    if(SysTick_Count > 0) {
        SysTick_Count--;
    }
}