/****************************************************************************
Author: Gabe DiMartino
Lab: Stepper Motor Control
Date Created: April 22, 2025
Description: Controls a unipolar stepper motor via TM4C123 on EduBase
****************************************************************************/

#include "TM4C123GH6PM.h"
#include "stepper.h"
#include "delay.h"

#define MOT1 0x01  // PF0
#define MOT2 0x02  // PF1
#define MOT3 0x04  // PF2
#define MOT4 0x08  // PF3

static int stepIndex = 0;
static int direction = 1;

const uint8_t stepSequence[4] = {
    MOT1 | MOT2,
    MOT2 | MOT3,
    MOT3 | MOT4,
    MOT4 | MOT1
};

void Stepper_Init(void) {
    SYSCTL->RCGCGPIO |= 0x20;         // Enable clock to GPIOF
    while ((SYSCTL->PRGPIO & 0x20) == 0);

    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= 0x0F;
    GPIOF->DIR |= 0x0F;               // PF0–PF3 as outputs
    GPIOF->DEN |= 0x0F;
    GPIOF->DATA &= ~0x0F;

    GPIOF->DATA |= (MOT3 | MOT4);
}

void Stepper_SetDirection(int dir) {
    direction = (dir >= 0) ? 1 : -1;
}

void Stepper_Step(int steps) {
    for (int i = 0; i < steps; i++) {
        stepIndex = (stepIndex + direction + 4) % 4;
        GPIOF->DATA = (GPIOF->DATA & ~0x0F) | stepSequence[stepIndex];
        delayMs(5);  // 5 ms delay between steps
    }
}

void Stepper_Seek(uint8_t desired, uint32_t time, uint8_t direction) {
    Stepper_SetDirection(direction);
    int totalSteps = desired * 16;  // 16 steps per cm
    for (int i = 0; i < totalSteps; i++) {
        stepIndex = (stepIndex + direction + 4) % 4;
        GPIOF->DATA = (GPIOF->DATA & ~0x0F) | stepSequence[stepIndex];
        delayMs(time);
    }
}

