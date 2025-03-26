#ifndef DAC_DRIVER_H
#define DAC_DRIVER_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

// Initialize SSI1 for DAC communication
void SSI1DAC_Init(void);

// Write data to DAC via SSI1
void SSI1DAC_Write(unsigned char data);

// Additional helper functions can be added here
void DAC_WriteVoltage(uint16_t voltage);

#endif // DAC_DRIVER_H