/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 4, 2025
Description: Implementation of reaction timer
****************************************************************************/

#include "ReactionTimer.h"

// Global variables for reaction timer
volatile uint32_t FirstCaptureTime = 0;
volatile uint32_t SecondCaptureTime = 0;
volatile uint32_t ReactionPeriod = 0;
volatile uint8_t TimerState = 0; // 0=ready, 1=waiting for second press, 2=completed
volatile uint8_t OutputReady = 0;

void PortF_Init(void) {
    SYSCTL->RCGCGPIO |= 0x20;
    while ((SYSCTL->PRGPIO & 0x20) == 0) {};
    
    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= 0x1F;
    GPIOF->DIR &= ~0x11;
    GPIOF->PUR |= 0x11;
    GPIOF->DIR |= 0x0E;
    GPIOF->DEN |= 0x1F;
    GPIOF->DATA &= ~0x0E;
}

void LED_Red_On(void) { GPIOF->DATA |= 0x02; }
void LED_Red_Off(void) { GPIOF->DATA &= ~0x02; }

void ReactionTimer_Init(void) {
    TimerState = 0;
    FirstCaptureTime = 0;
    SecondCaptureTime = 0;
    ReactionPeriod = 0;
    OutputReady = 0;
    
    PortF_Init();
    
    SYSCTL->RCGCTIMER |= 0x03;
    SYSCTL->RCGCGPIO |= 0x02;
    while ((SYSCTL->PRGPIO & 0x02) == 0) {};
    
    GPIOB->DIR &= ~0x50;
    GPIOB->AFSEL |= 0x50;
    GPIOB->DEN |= 0x50;
    GPIOB->PCTL = (GPIOB->PCTL & 0xFF0F00FF) | 0x07070000;
    
    TIMER0->CTL &= ~0x01;
    TIMER0->CFG = 0x04;
    TIMER0->TAMR = 0x07;
    TIMER0->CTL |= 0x0C;
    TIMER0->TAILR = 0xFFFF;
    TIMER0->TAPR = 0xFF;
    TIMER0->IMR |= 0x04;
    TIMER0->ICR = 0x04;
    
    TIMER1->CTL &= ~0x01;
    TIMER1->CFG = 0x04;
    TIMER1->TAMR = 0x07;
    TIMER1->CTL |= 0x0C;
    TIMER1->TAILR = 0xFFFF;
    TIMER1->TAPR = 0xFF;
    TIMER1->IMR |= 0x04;
    TIMER1->ICR = 0x04;
    
    NVIC->IP[19] = 2 << 5;
    NVIC->IP[21] = 2 << 5;
    NVIC->ISER[0] = (1 << 19) | (1 << 21);
    
    TIMER0->CTL |= 0x01;
    __enable_irq();
}

void TIMER0A_Handler(void) {
    TIMER0->ICR = 0x04; // Clear interrupt flag
    if (TimerState == 0) {
        FirstCaptureTime = TIMER0->TAR;
        TimerState = 1;
        LED_Red_On();
        TIMER0->CTL &= ~0x01; // Disable Timer0
        TIMER1->ICR = 0x04;   // Clear Timer1 flag
        TIMER1->CTL |= 0x01;  // Enable Timer1
    }
}

void TIMER1A_Handler(void) {
    TIMER1->ICR = 0x04; // Clear interrupt flag
    if (TimerState == 1) {
        SecondCaptureTime = TIMER1->TAR;

        // Handle wraparound case
        ReactionPeriod = SecondCaptureTime;
        
        TimerState = 2;
        OutputReady = 1;
        LED_Red_Off();
        TIMER1->CTL &= ~0x01; // Disable Timer1
    }
}

uint32_t ReactionTime_Get(void) {
    if (OutputReady) {
        OutputReady = 0;
        return ((ReactionPeriod) * 20) / 1000000;  // Convert to milliseconds
    } else {
        return 0xFFFFFFFF;
    }
}

uint8_t ReactionTimer_Status(void) { return TimerState; }

void ReactionTimer_Reset(void) {
    TimerState = 0;
    FirstCaptureTime = 0;
    SecondCaptureTime = 0;
    ReactionPeriod = 0;
    OutputReady = 0;
    LED_Red_Off();
    TIMER0->ICR = 0x04;
    TIMER1->ICR = 0x04;
    TIMER1->CTL &= ~0x01;
    TIMER0->CTL |= 0x01;
}

