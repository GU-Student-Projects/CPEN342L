/****************************************************************************
Author: Gabe DiMartino
Lab: Stepper Motor Control
Date Created: April 22, 2025
Description: Controls a unipolar stepper motor via TM4C123 on EduBase
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "stepper.h"
#include "delay.h"

// Step sequence output on PB3–PB0
const uint8_t stepSequence[4] = {
    0x05,  // 0101 → A4 + A2
    0x06,  // 0110 → A3 + A2
    0x0A,  // 1010 → A3 + A1
    0x09   // 1001 → A4 + A1
};

static int stepIndex = 0;
static int direction = 1;

void Stepper_Init(void) {
    // Enable clocks to GPIOB and GPIOF
    SYSCTL->RCGCGPIO |= (1U << 1) | (1U << 5);  // Port B and Port F
    while ((SYSCTL->PRGPIO & ((1U << 1) | (1U << 5))) == 0);

    // Set PB0–PB3 as outputs for step control
    GPIOB->DIR |= 0x0F;
    GPIOB->DEN |= 0x0F;
    GPIOB->DATA &= ~0x0F;

    // Set PF2 and PF3 as outputs for PWM (set high)
    GPIOF->DIR |= (1U << 2) | (1U << 3);
    GPIOF->DEN |= (1U << 2) | (1U << 3);
    GPIOF->DATA |= (1U << 2) | (1U << 3);  // Force PWM pins high for now
}

void Stepper_SetDirection(int dir) {
    direction = (dir >= 0) ? 1 : -1;
}

void Stepper_Step(int steps) {
    for (int i = 0; i < steps; i++) {
        stepIndex = (stepIndex + direction + 4) % 4;
        GPIOB->DATA = (GPIOB->DATA & ~0x0F) | stepSequence[stepIndex];
        delayMs(5);
    }
}

void Stepper_Seek(uint8_t desired, uint32_t time, uint8_t dir) {
    Stepper_SetDirection(dir);
    int totalSteps = desired * 16;  // Assuming 16 steps per cm
    for (int i = 0; i < totalSteps; i++) {
        stepIndex = (stepIndex + direction + 4) % 4;
        GPIOB->DATA = (GPIOB->DATA & ~0x0F) | stepSequence[stepIndex];
        delayMs(time);
    }
}
