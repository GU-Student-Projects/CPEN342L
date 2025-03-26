#include "dac_driver.h"
#include "DIPSwitch.h"
#include "LCD.h"

// Function to convert hex DIP value to voltage string for LCD display
void hex2dec(uint8_t x, char *str) {
    if (x == 0) {
        str[0] = '0';
        str[1] = '0';
        str[2] = 'V';
    } else {
        str[0] = '0' + (x / 10);   // Tens place
        str[1] = '0' + (x % 10);   // Ones place
        str[2] = 'V';              // Voltage unit
    }
    str[3] = '\0';                 // Null-terminate string
}

// Scale DIP switch input to DAC range (0-15V mapped to 12-bit DAC)
uint16_t Scale_Voltage(uint8_t dip_value) {
    return (dip_value * 4095) / 15;  // Map DIP switch range (0-15) to DAC (0-4095)
}

int main(void) {
    char voltage_str[4];
    uint8_t dip_value;
    uint8_t prev_value = 0xFF;  // Initialize to an invalid value
    uint16_t dac_value;

    // Initialize all peripherals
    DIPSwitch_Init();
    LCD_init();
    SSI1DAC_Init();

    while (1) {
        // Read 4-bit DIP switch value
        dip_value = DIPSwitch_Read() & 0x0F;

        if (dip_value != prev_value) {
            prev_value = dip_value;

            // Convert DIP switch value to DAC output
            dac_value = Scale_Voltage(dip_value);

            // Transmit correctly formatted DAC data (16-bit command)
            unsigned char highByte = (0x4 << 4) | ((dac_value >> 8) & 0x0F);
            unsigned char lowByte = dac_value & 0xFF;

            GPIOD->DATA |= 0x02;  // Pull FS low
            GPIOD->DATA &= ~0x02; 

            SSI1DAC_Write(highByte);  // Send upper 8 bits (with control bits)
            SSI1DAC_Write(lowByte);   // Send lower 8 bits
					
						GPIOD->DATA |= 0x02;

            // Prepare LCD display string
            hex2dec(dip_value, voltage_str);

            // Update LCD display
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str(voltage_str);

            // Small delay to prevent flickering
            for (volatile int i = 0; i < 10000; i++);
        }
    }
    return 0;
}
