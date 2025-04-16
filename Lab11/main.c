#include "TM4C123GH6PM.h"
#include "adc_driver.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include "stdio.h"

#define THRESHOLD 2000
#define TIMEOUT 1000000

typedef struct {
    float period_us;
    float frequency_hz;
    float duty_cycle;
    float high_v;
    float low_v;
    float average_v;
} SignalInfo;

SignalInfo measure_signal(void);
void display_info(uint8_t key, SignalInfo data);
float adc_to_voltage(uint16_t val);

int main(void) {
    // Initialize peripherals
    LCD_init();
    MatrixKeypad_Init();
    ADC1_InitCh2();
    
    LCD_Clear(); // Clear display
    
    char lastKey = 0;  // To keep track of the last key pressed
    
    while (1) {
        // Scan keypad directly for faster response
        char key = MatrixKeypad_Scan();
        
        // Only update display when a new key is pressed
        if (key != 0 && key != lastKey) {
            lastKey = key;
            SignalInfo data = measure_signal();
            display_info(key, data);
        }
        
        // Reset lastKey when no key is pressed
        if (key == 0) {
            lastKey = 0;
        }
        
        // Small delay to prevent excessive CPU usage
        delayMs(10);
    }
}

SignalInfo measure_signal(void) {
    uint32_t cntH = 0, cntL = 0;
    uint16_t val;
    uint16_t high = 0, low = 4095;
    uint32_t sum = 0;
    uint32_t samples = 0;
    uint32_t timeout = TIMEOUT;
    
    // Wait for rising edge or timeout
    do {
        val = ADC1_InCh2();
        if (--timeout == 0) break;
    } while (val < THRESHOLD);
    
    // Measure high time
    timeout = TIMEOUT;
    while (val >= THRESHOLD && timeout--) {
        if (val > high) high = val;
        sum += val;
        cntH++;
        val = ADC1_InCh2();
        samples++;
    }
    
    // Measure low time
    timeout = TIMEOUT;
    while (val < THRESHOLD && timeout--) {
        if (val < low) low = val;
        sum += val;
        cntL++;
        val = ADC1_InCh2();
        samples++;
    }
    
    // Calculate signal parameters
    float period = ((float)(cntH + cntL) / 250000.0f) * 1e6f; // Convert to microseconds
    float freq = 1000000.0f / period;
    float duty = (float)cntH / (cntH + cntL);
    float v_high = adc_to_voltage(high);
    float v_low = adc_to_voltage(low);
    float v_avg = duty * v_high + (1.0f - duty) * v_low; // Calculate average voltage like in working code
    
    SignalInfo result = {
        .period_us = period,
        .frequency_hz = freq,
        .duty_cycle = duty * 100.0f, // Convert to percentage
        .high_v = v_high,
        .low_v = v_low,
        .average_v = v_avg
    };
    
    return result;
}

float adc_to_voltage(uint16_t val) {
    float v_adc = (val * 3.3f) / 4095.0f; // ADC gives 0-3.3V
    // Match the voltage calculation from the working code
    float v_signal = (v_adc * 3.0f) - (9.9f / 2.0f);
    return v_signal;
}

void display_info(uint8_t key, SignalInfo data) {
    char buf[20];
    
    LCD_Clear(); // Clear display
    
    switch (key) {
        case '1':
            LCD_Str("Period");
            sprintf(buf, "%.2fus", data.period_us);
            break;
        case '2':
            LCD_Str("Freq");
            sprintf(buf, "%.2fHz", data.frequency_hz);
            break;
        case '3':
            LCD_Str("DutyCycle");
            sprintf(buf, "%.2f%%", data.duty_cycle);
            break;
        case '4':
            LCD_Str("HighVolts");
            sprintf(buf, "%.2fV", data.high_v);
            break;
        case '5':
            LCD_Str("LowVolts");
            sprintf(buf, "%.2fV", data.low_v);
            break;
        case '6':
            LCD_Str("AvgVolts");
            sprintf(buf, "%.2fV", data.average_v);
            break;
        default:
            LCD_Str("ERROR");
            sprintf(buf, "");
            break;
    }
    
    // Move to second line and display the value
    LCD_SetCursor(1, 0); // row 1 (second row), column 0 (first position)
    LCD_Str(buf);
}