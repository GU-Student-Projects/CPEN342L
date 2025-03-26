#include "dac_driver.h"

int main(void) {
    SSI1DAC_Init();   // Initialize SPI interface for DAC
    
    while (1) {
        // Rising edge of triangle wave: 0 to 4095
        for (int dac_value = 0; dac_value <= 4095; dac_value += 10) { // Step size of 10 for smoothness
						unsigned char highByte, lowByte;
            // Generate proper DAC command
            highByte = (0x4 << 4) | ((dac_value >> 8) & 0x0F);
            lowByte = dac_value & 0xFF;

            GPIOD->DATA |= 0x02;  // Pull FS low
            GPIOD->DATA &= ~0x02; 

            SSI1DAC_Write(highByte);
            SSI1DAC_Write(lowByte);
            
            // Small delay (adjust as needed for frequency control)
            for (volatile int i = 0; i < 1000; i++);
        }

        // Falling edge of triangle wave: 4095 to 0
        for (int dac_value = 4095; dac_value > 0; dac_value -= 10) {
						unsigned char highByte, lowByte;
            // Generate proper DAC command
            highByte = (0x4 << 4) | ((dac_value >> 8) & 0x0F);
            lowByte = dac_value & 0xFF;

            GPIOD->DATA |= 0x02;  // Pull FS low
            GPIOD->DATA &= ~0x02; 

            SSI1DAC_Write(highByte);
            SSI1DAC_Write(lowByte);
            
            // Small delay (adjust as needed for frequency control)
            for (volatile int i = 0; i < 1000; i++);
        }
    }
}
