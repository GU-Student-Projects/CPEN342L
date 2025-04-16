#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

// Constants
#define THRESHOLD 2060
#define ADC_CHANNEL 8
#define SAMPLE_OFFSET 50

// Function prototypes
void ADC0_InitCh8(void);
uint32_t sampleReading(void);
void findValues(uint32_t *high, uint32_t *low, uint32_t *count, uint32_t *disp);
uint32_t sampleHighDuration(void);
uint32_t samplePeriodDuration(void);

#endif /* ADC_DRIVER_H */