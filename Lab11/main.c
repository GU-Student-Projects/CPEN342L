/**************************************************************************** 
Author: Gabe DiMartino 
Lab: Digital Signal Analyzer 
Date Created: April 15, 2025 
Last Modified: April 15, 2025 
Description: Implementation of DSA

****************************************************************************/  

#include "TM4C123GH6PM.h"
#include "adc_driver.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include "stdio.h"

#define THRESHOLD 2000
#define MAX_SAMPLES 10000
#define TIMEOUT_LIMIT MAX_SAMPLES
#define SAMPLE_DELAY_US 50

// Struct to hold signal data
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
    ADC1_InitCh8();     // Using PE5 (AIN8)
    MatrixKeypad_Init();
    LCD_init();
    LCD_Clear();

    SignalInfo data;

    char key = 0;
    static char lastKey = 0;
    static uint8_t keyHeld = 0;

    while (1) {
        key = MatrixKeypad_Scan();

        if (key && !keyHeld) {
            keyHeld = 1;
            lastKey = key;

            LCD_Clear();
            data = measure_signal();
            display_info(key, data);
        }

        if (!key) {
            keyHeld = 0;
        }
    }
}


// Measures a square wave signal and returns relevant information
SignalInfo measure_signal(void) {
    uint32_t cntH = 0, cntL = 0;
    uint16_t val;
    float high = 0, low = 4095, sum = 0;
    uint32_t samples = 0;
    uint32_t timeout = 0;

    // Ensure signal is low before starting
    timeout = 0;
    do {
        val = ADC1_InCh8();
        if (++timeout > TIMEOUT_LIMIT) goto flatline;
        delayUs(SAMPLE_DELAY_US);
    } while (val >= THRESHOLD);

    // Wait for rising edge (low to high)
    timeout = 0;
    do {
        val = ADC1_InCh8();
        if (++timeout > TIMEOUT_LIMIT) goto flatline;
        delayUs(SAMPLE_DELAY_US);
    } while (val < THRESHOLD);

    // Count high time
    timeout = 0;
    while (val >= THRESHOLD && timeout++ < TIMEOUT_LIMIT) {
        if (val > high) high = val;
        sum += val;
        cntH++;
        samples++;
        delayUs(SAMPLE_DELAY_US);
        val = ADC1_InCh8();
    }

    // Count low time
    timeout = 0;
    while (val < THRESHOLD && timeout++ < TIMEOUT_LIMIT) {
        if (val < low) low = val;
        sum += val;
        cntL++;
        samples++;
        delayUs(SAMPLE_DELAY_US);
        val = ADC1_InCh8();
    }

    if (cntH + cntL == 0) goto flatline;

    float total_samples = (float)(cntH + cntL);
    float period = (double)((total_samples) * SAMPLE_DELAY_US) * 1000;
    float freq = (1000000.0f / period);
    float duty = (cntH * 100.0f) / total_samples;
    float v_high = (((double) high * 3.3) / 4095)*3-(9.9/2);
    float v_low = ((((double) low * 3.3) / 4095)*3-(9.9/2));
    float v_avg = adc_to_voltage((uint16_t)(sum / samples));

    SignalInfo result = { period, freq, duty, v_high, v_low, v_avg };
    return result;

flatline:
    // Return default struct indicating no signal
    SignalInfo fail = {0, 0, 0, 0, 0, 0};
    LCD_command(1);
    LCD_Str("No signal...");
    return fail;
}


// Converts 12-bit ADC value to voltage assuming 3.3V reference
float adc_to_voltage(uint16_t val) {
    return (val * 3.3f) / 4095.0f;
}

// Displays signal info based on key input
void display_info(uint8_t key, SignalInfo data) {
    char str[20];

    switch (key) {
        case '4':
            sprintf(str, "Per: %.2f us", data.period_us);
            break;
        case '5':
            sprintf(str, "Freq: %.2f Hz", data.frequency_hz);
            break;
        case '6':
            sprintf(str, "Duty: %.2f %%", data.duty_cycle);
            break;
        case '7':
            sprintf(str, "High: %.2f V", data.high_v);
            break;
        case '8':
            sprintf(str, "Low: %.2f V", data.low_v);
            break;
        case '9':
            sprintf(str, "Avg: %.2f V", data.average_v);
            break;
        default:
            sprintf(str, "Invalid key: %c", key);
            break;
    }

    LCD_Str(str);
}
