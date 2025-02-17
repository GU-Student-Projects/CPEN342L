// trafficlight.h
#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "delay.h"

#define NS_RED (1<<0)  // PB0: North-South Red
#define NS_YEL (1<<1)  // PB1: North-South Yellow
#define NS_GRN (1<<2)  // PB2: North-South Green
#define EW_RED (1<<3)  // PB3: East-West Red
#define EW_YEL (1<<4)  // PB4: East-West Yellow
#define EW_GRN (1<<5)  // PB5: East-West Green

// State definitions
typedef enum {
    NS_GREEN_EW_RED = 0,  // State 0
    NS_YEL_EW_RED = 1,    // State 1
    NS_RED_EW_RED_1 = 2,  // State 2
    NS_RED_EW_GREEN = 3,  // State 3
    NS_RED_EW_YEL = 4,    // State 4
    NS_RED_EW_RED_2 = 5   // State 5
} TrafficState;

void TrafficLight_Init(void);
void TrafficLight_Update(void);
void PLL_Init(void);

#endif