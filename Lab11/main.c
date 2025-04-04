#include "TM4C123GH6PM.h"
#include "adc_driver.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include "stdio.h"

#define THRESHOLD 2000

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
    ADC1_InitCh2();
    LCD_init();
    Keypad_init();
    LCD_command(1);

    while (1) {
        SignalInfo data = measure_signal();
        uint8_t key = Keypad_getKey();  // Poll or interrupt-based

        display_info(key, data);
        delayMs(500);
    }
}

// Measures a square wave signal and returns relevant information
SignalInfo measure_signal(void) {
    uint32_t cntH = 0, cntL = 0;
    uint16_t val;
    float high = 0, low = 4095, sum = 0;
    uint32_t samples = 0;

    // Wait for signal to stabilize
    do {
        val = ADC1_InCh2();
    } while (val < THRESHOLD);

    // Count one full cycle
    while (val >= THRESHOLD) {
        if (val > high) high = val;
        sum += val;
        cntH++;
        val = ADC1_InCh2();
        samples++;
    }

    while (val < THRESHOLD) {
        if (val < low) low = val;
        sum += val;
        cntL++;
        val = ADC1_InCh2();
        samples++;
    }

    float period = ((float)(cntH + cntL) / 250000.0f) * 1e6f;
    float freq = 1000000.0f / period;
    float duty = ((float)cntH / (cntH + cntL)) * 100.0f;
    float v_high = adc_to_voltage((uint16_t)high);
    float v_low = adc_to_voltage((uint16_t)low);
    float v_avg = adc_to_voltage((uint16_t)(sum / samples));

    SignalInfo result = {
        .period_us = period,
        .frequency_hz = freq,
        .duty_cycle = duty,
        .high_v = v_high,
        .low_v = v_low,
        .average_v = v_avg
    };

    return result;
}

// Converts 12-bit ADC value to voltage assuming 3.3V range
float adc_to_voltage(uint16_t val) {
    return (val * 3.3f) / 4095.0f;
}

// Displays signal info based on key input
void display_info(uint8_t key, SignalInfo data) {
    char str[20];
    LCD_command(1);

    switch (key) {
        case '1':
            sprintf(str, "Period: %.2f us", data.period_us);
            break;
        case '2':
            sprintf(str, "Freq: %.2f Hz", data.frequency_hz);
            break;
        case '3':
            sprintf(str, "Duty: %.2f %%", data.duty_cycle);
            break;
        case '4':
            sprintf(str, "High: %.2f V", data.high_v);
            break;
        case '5':
            sprintf(str, "Low: %.2f V", data.low_v);
            break;
        case '6':
            sprintf(str, "Avg: %.2f V", data.average_v);
            break;
        default:
            sprintf(str, "Invalid key: %c", key);
            break;
    }

    LCD_Str(str);
}
