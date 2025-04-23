/****************************************************************************
Author: Gabe DiMartino
Lab: TM4C Based ohmmeter
Date Created: March 18, 2025
Last Modified: March 18, 2025
Description:ommeter main
****************************************************************************/

#include "dac_driver.h"
#include "LCD.h"
#include "tm4c123gh6pm.h"
#include "delay.h"
#include <stdio.h>

// Initialize Port A, bit 7 (PA7) as input for comparator output
void PortA_Init(void) {
    // Enable clock for Port A
    SYSCTL->RCGCGPIO |= 0x01;
    // Wait for clock to stabilize
    while((SYSCTL->RCGCGPIO & 0x01) == 0) {}
    
    // Configure PA7 as digital input
    GPIOA->DIR &= ~0x80;    // Set PA7 as input (bit 7 = 0)
    GPIOA->DEN |= 0x80;     // Enable digital function for PA7
}

// Initialize Port F, bit 4 (PF4) as input for SW1 pushbutton
void PortF_Init(void) {
    // Enable clock for Port F
    SYSCTL->RCGCGPIO |= 0x20;
    // Wait for clock to stabilize
    while((SYSCTL->PRGPIO & 0x20) == 0) {}
    
    // Unlock Port F configuration registers
    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= 0x10;      // Allow changes to PF4
    
    // Configure PF4 as digital input with pull-up resistor
    GPIOF->DIR &= ~0x10;    // Set PF4 as input (bit 4 = 0)
    GPIOF->PUR |= 0x10;     // Enable pull-up resistor for PF4
    GPIOF->DEN |= 0x10;     // Enable digital function for PF4
    
    // Complete configuration by setting control register back
    GPIOF->CR = 0x00;
    GPIOF->LOCK = 0;
}

// Convert resistance value to display string
void resistance_to_str(uint32_t resistance, char *str) {
    // Handle different ranges with appropriate units
    sprintf(str, "%4d Ohm", resistance);
}

int main(void) {
    // Initialize all required peripherals
    SSI1DAC_Init();
    PortA_Init();
    PortF_Init();
    LCD_init();
    
    // Display startup message
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Str("Ohmmeter Ready");
    LCD_SetCursor(1, 0);
    LCD_Str("Press SW1");
    
    // Variables for resistance measurement
    uint16_t index = 0;
    uint32_t resistorValue = 0;
    char resistance_str[12];
    uint8_t measurement_active = 0;
    
    // Define R_B value in ohms (from lab calculation)
    const uint16_t R_B = 240;
    
    while (1) {
        // Check if SW1 is pressed (active low)
        if ((GPIOF->DATA & 0x10) == 0) {
            // Button pressed - start new measurement
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str("Measuring...");
            
            // Reset index and start measurement
            index = 0;
            measurement_active = 1;
            
            // Debounce delay
            delayMs(200);
        }
        
        // If measurement is active, perform it
        if (measurement_active) {
            // Start from index 1 (avoid division by zero)
            index = 1;
            
            // Perform binary search
            uint16_t min_index = 1;
            uint16_t max_index = 4095;
            uint16_t mid_index;
            
						// Start from index 1 to avoid division by zero
						while (index <= 4095) {
								// Send DAC value
								GPIOD->DATA |= 0x02;  // Pull FS high
								GPIOD->DATA &= ~0x02; // Pull FS low
								
								unsigned char highByte = (0x4 << 4) | ((index >> 8) & 0x0F);
								unsigned char lowByte = index & 0xFF;
								
								SSI1DAC_Write(highByte);
								SSI1DAC_Write(lowByte);
								
								// Small delay to allow circuit to stabilize
								delayMs(5);
								
								// Check comparator output
								if ((GPIOA->DATA & 0x80) == 0) {
										// Comparator output went high -> we found the threshold
										break;
								}
								
								// Increment index for next step
								index++;
						}

            
            // Calculate resistance based on the final index
            // Using formula: R_L = (2 * R_B * 4096) / (5 * index)
            resistorValue = (2 * R_B * 4096) / (5 * index);
            
            // Display the result
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str("Resistance:");
            
            // Convert resistance to string and display
            resistance_to_str(resistorValue, resistance_str);
            LCD_SetCursor(1, 0);
            LCD_Str(resistance_str);
            
            // Measurement complete
            measurement_active = 0;
            
            // Wait for button press to reset and start new measurement
            // First wait for button release (if still pressed)
            while ((GPIOF->DATA & 0x10) == 0) {
                delayMs(10);
            }
            
            // Now wait for next button press to reset
            LCD_SetCursor(1, 0);
            LCD_Str(resistance_str);
            LCD_SetCursor(1, 12);
            LCD_Str("RST");
            
            // Wait indefinitely until button is pressed again
            while ((GPIOF->DATA & 0x10) != 0) {
                // Just waiting for button press
            }
            
            // Button pressed - prepare for next measurement
            delayMs(200);  // Debounce
        }
    }
}
