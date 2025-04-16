#include "adc_driver.h"

void ADC0_InitCh8(void) {
    volatile uint32_t delay;
    
    // Activate clocks
    SYSCTL->RCGCADC |= 0x01;      // Activate ADC0
    SYSCTL->RCGCGPIO |= 0x10;     // Activate clock for Port E
    delay = SYSCTL->RCGCGPIO;     // Allow clock to stabilize
    
    // Configure Port E pin 5 (AIN8)
    GPIOE->DIR   &= ~0x20;        // Make PE5 input
    GPIOE->AFSEL |=  0x20;        // Enable alternate function
    GPIOE->DEN   &= ~0x20;        // Disable digital I/O
    GPIOE->AMSEL |=  0x20;        // Enable analog functionality
    
    // Configure ADC0 sequencer 2
    ADC0->PC     = 0x05;          // 500K samples/sec
    ADC0->SSPRI  = 0x3210;        // Set sequencer priorities
    ADC0->ACTSS &= ~0x0004;       // Disable sequencer 2 before config
    ADC0->EMUX  &= ~0x0F00;       // Software trigger
    ADC0->SSMUX2 = ADC_CHANNEL;   // Select AIN8
    ADC0->SSCTL2 = 0x0006;        // Set IE0 and END0 bits
    ADC0->IM    &= ~0x0004;       // Disable interrupts
    ADC0->ACTSS |=  0x0004;       // Enable sequencer 2
}

// Optimized ADC sample reading
inline uint32_t sampleReading(void) {
    ADC0->PSSI = 0x0004;                 // Initiate SS2
    while(!(ADC0->RIS & 0x04)) { }       // Wait for conversion (optimized)
    uint32_t result = ADC0->SSFIFO2 & 0xFFF; // Read 12-bit result
    ADC0->ISC = 0x0004;                  // Clear completion flag
    return result;
}

// Skip a fixed number of samples
inline void skipSamples(int count) {
    for (int i = 0; i < count; i++) {
        sampleReading();
    }
}

// Wait for signal to cross threshold (rising or falling)
inline void waitForCrossing(uint8_t rising) {
    uint32_t reading;
    if (rising) {
        // Wait for rising edge
        while ((reading = sampleReading()) < THRESHOLD) { }
    } else {
        // Wait for falling edge
        while ((reading = sampleReading()) >= THRESHOLD) { }
    }
}

void findValues(uint32_t *high, uint32_t *low, uint32_t *count, uint32_t *disp) {
    // Wait for rising edge
    waitForCrossing(1);
    
    // Skip offset samples
    skipSamples(SAMPLE_OFFSET);
    
    // Read high value
    *high = sampleReading();
    
    // Wait for falling edge
    waitForCrossing(0);
    
    // Skip offset samples
    skipSamples(SAMPLE_OFFSET);
    
    // Read low value
    *low = sampleReading();
    
    // Initialize count and display flags
    *count = 0;
    *disp = 1;
}

uint32_t sampleHighDuration(void) {
    uint32_t count = 0;
    uint32_t reading = sampleReading();
    
    // Handle case where signal is already high
    if (reading > THRESHOLD) {
        // Wait for falling edge
        waitForCrossing(0);
        
        // Skip offset samples
        skipSamples(SAMPLE_OFFSET);
    }
    
    // Wait for rising edge
    waitForCrossing(1);
    
    // Count samples during high period
    do {
        count++;
        reading = sampleReading();
    } while (reading >= THRESHOLD);
    
    return count;
}

uint32_t samplePeriodDuration(void) {
    uint32_t count = 0;
    
    // Wait for rising edge
    waitForCrossing(1);
    
    // Count high period samples
    while (sampleReading() >= THRESHOLD) {
        count++;
    }
    
    // Count low period samples
    while (sampleReading() < THRESHOLD) {
        count++;
    }
    
    return count;
}