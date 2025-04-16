// adc_driver.h

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

// Initializes ADC1 to read analog input channel 2 (PE1 / AIN2)
void ADC1_InitCh8(void);

// Reads from ADC1 channel 2 (PE1 / AIN2) and returns 12-bit result
uint16_t ADC1_InCh8(void);

#endif // ADC_DRIVER_H
