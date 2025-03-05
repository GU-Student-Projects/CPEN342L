/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 5, 2025
Description: Prototype of reaction timer
****************************************************************************/
#ifndef REACTIONTIMER_H
#define REACTIONTIMER_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

// Function prototypes for Reaction Timer
void ReactionTimer_Init(void);          // Initialize timers and ports for reaction time measurement
uint32_t ReactionTime_Get(void);        // Get the measured reaction time in ms
uint8_t ReactionTimer_Status(void);     // Get the status of the reaction timer (0: ready, 1: waiting for second button, 2: measurement complete)
void ReactionTimer_Reset(void);         // Reset the reaction timer for a new measurement

// Function prototypes for LED control
void PortF_Init(void);                  // Initialize Port F for LEDs and switches
void LED_Red_On(void);                  // Turn on red LED (PF1)
void LED_Red_Off(void);                 // Turn off red LED (PF1)

// Function prototypes for switches
uint32_t Switch_Input1(void);           // Read SW1 (PF4)
uint32_t Switch_Input2(void);           // Read SW2 (PF0)

// External declarations for global variables
extern volatile uint32_t FirstCaptureTime;  // Time of first button press
extern volatile uint32_t SecondCaptureTime; // Time of second button press
extern volatile uint32_t ReactionPeriod;    // Time difference between captures
extern volatile uint32_t OutputReady;       // Flag to indicate measurement is ready
extern volatile uint8_t TimerState;         // 0=ready, 1=waiting for second press, 2=completed

#endif // REACTIONTIMER_H
