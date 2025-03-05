/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 4, 2025
Description: Protype of reaction timer
****************************************************************************/

#ifndef REACTIONTIMER_H
#define REACTIONTIMER_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

// Function prototypes
void ReactionTimer_Init(void);     // Initialize timers and ports for reaction time measurement
void LED_Init(void);               // Initialize LED on the LaunchPad
void LED_On(void);                 // Turn on LED
void LED_Off(void);                // Turn off LED
uint32_t ReactionTime_Get(void);   // Get the measured reaction time in ms
uint8_t ReactionTime_Status(void); // Get the status of the reaction timer (0: ready, 1: waiting for second button, 2: measurement complete)
void ReactionTime_Reset(void);     // Reset the reaction timer for a new measurement

#endif // REACTIONTIMER_H