#include "TM4C123GH6PM.h"
#include "adc_driver.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include "stdio.h"

#define THRESHOLD 2000
#define MAX_SAMPLES 10000
#define SAMPLE_DELAY_US 4

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
    LCD_Init();
    LCD_Clear();

    SignalInfo data;

    char key = 0;
    static char lastKey = 0;
    static uint8_t keyHeld = 0;

    while (1) {
        key = MatrixKeypad_Scan();  // Lower-level scan gives more control

        if (key && !keyHeld) {
            keyHeld = 1;
            lastKey = key;

            LCD_Clear();
            data = measure_signal();
            display_info(key, data);
        }

        if (!key) {
            keyHeld = 0;  // Reset when key is released
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

    // Wait for rising edge (low to high)
    do {
        val = ADC1_InCh8();
        if (++timeout > MAX_SAMPLES) goto flatline;
    } while (val < THRESHOLD);

    timeout = 0;
    // Count high time
    while (val >= THRESHOLD && timeout++ < MAX_SAMPLES) {
        if (val > high) high = val;
        sum += val;
        cntH++;
        samples++;
        val = ADC1_InCh8();
        delayUs(SAMPLE_DELAY_US);  // Optional control
    }

    timeout = 0;
    // Count low time
    while (val < THRESHOLD && timeout++ < MAX_SAMPLES) {
        if (val < low) low = val;
        sum += val;
        cntL++;
        samples++;
        val = ADC1_InCh8();
        delayUs(SAMPLE_DELAY_US);  // Optional control
    }

    if (cntH + cntL == 0) goto flatline;

    float period = ((float)(cntH + cntL) * SAMPLE_DELAY_US); // In µs
    float freq = 1000000.0f / period;
    float duty = ((float)cntH / (cntH + cntL)) * 100.0f;
    float v_high = adc_to_voltage((uint16_t)high);
    float v_low = adc_to_voltage((uint16_t)low);
    float v_avg = adc_to_voltage((uint16_t)(sum / samples));

    SignalInfo result = { period, freq, duty, v_high, v_low, v_avg };
    return result;

flatline:
    // Return default struct indicating no signal
    SignalInfo fail = {0, 0, 0, 0, 0, 0};
    return fail;
}


// Converts 12-bit ADC value to voltage assuming 3.3V reference
float adc_to_voltage(uint16_t val) {
    return (val * 3.3f) / 4095.0f;
}

// Displays signal info based on key input
void display_info(uint8_t key, SignalInfo data) {
    char str[20];
    LCD_command(1);  // Clear screen

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
