/****************************************************************************
Author: Gabe DiMartino
Lab: Periodic Interrupt-Driven Traffic Lights
Date Created: February 16, 2025
Last Modified: February 16, 2025

Description: Traffic Light LED interface functions.
****************************************************************************/
#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "delay.h"

// Pin definitions for traffic lights (PB0-PB5)
#define NS_RED  (1<<0)  // PB0
#define NS_YEL  (1<<1)  // PB1
#define NS_GRN  (1<<2)  // PB2
#define EW_RED  (1<<3)  // PB3
#define EW_YEL  (1<<4)  // PB4
#define EW_GRN  (1<<5)  // PB5

typedef enum {
    NS_GREEN_EW_RED,    // State 0: North-South Green, East-West Red (10s)
    NS_YEL_EW_RED,      // State 1: North-South Yellow, East-West Red (2s)
    NS_RED_EW_RED_1,    // State 2: All Red (1s)
    NS_RED_EW_GREEN,    // State 3: North-South Red, East-West Green (10s)
    NS_RED_EW_YEL,      // State 4: North-South Red, East-West Yellow (2s)
    NS_RED_EW_RED_2     // State 5: All Red (1s)
} TrafficState;

void TrafficLight_Init(void);

#endif
