#include "TM4C123GH6PM.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include "stdio.h"
#include "adc_driver.h"

typedef struct {
    double period_us;
    double frequency_hz;
    double duty_cycle;
    double high_v;
    double low_v;
    double average_v;
} SignalInfo;

SignalInfo measure_signal(void);
void display_info(uint8_t key, SignalInfo data);

int main(void) {
    // Initialize peripherals
    LCD_init();
    MatrixKeypad_Init();
    ADC0_InitCh8();  // Initialize ADC using our optimized driver
    
    LCD_Clear(); // Clear display
    
    char lastKey = 0;  // Track the last key pressed
    
    while (1) {
        // Scan keypad for faster response
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
    uint32_t high, low, count, disp;
    
    // Get high and low voltage samples
    findValues(&high, &low, &count, &disp);
    
    // Get time measurements
    uint32_t highTime = sampleHighDuration();
    uint32_t periodSamples = samplePeriodDuration();
    
    // Calculate signal parameters using 2 microseconds per sample
    double period = (double)(periodSamples * 2.0);
    double frequency = 1e6 / period;
    double dutyCycle = (double)highTime / (double)periodSamples;
    
    // Calculate voltages using the formula from the working code
    double highVoltage = (((double)high * 3.3) / 4095.0) * 3.0 - (9.9 / 2.0);
    double lowVoltage = (((double)low * 3.3) / 4095.0) * 3.0 - (9.9 / 2.0);
    double averageVoltage = dutyCycle * highVoltage + (1.0 - dutyCycle) * lowVoltage;
    
    SignalInfo result = {
        .period_us = period,
        .frequency_hz = frequency,
        .duty_cycle = dutyCycle * 100.0,  // Convert to percentage
        .high_v = highVoltage,
        .low_v = lowVoltage,
        .average_v = averageVoltage
    };
    
    return result;
}

void convertSamplesToPeriod(int samples, char *buffer) {
    float period_us = ((float)samples / 250000.0f) * 1e6f; // 250kHz sample rate
    sprintf(buffer, "%.2f us", period_us);
}

void display_info(uint8_t key, SignalInfo data) {
    char buf[20];
    
    LCD_Clear();
    
    switch (key) {
        case '1':
            LCD_Str("Period");
            convertSamplesToPeriod((int)(data.period_us * 250.0 / 1000.0), buf);
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
    
    // Use LCD_SetCursor to move to the second line
    LCD_SetCursor(1, 0);
    LCD_Str(buf);
}