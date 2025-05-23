/****************************************************************************
Author: Gabe DiMartino
Lab: Input Capture Functions
Date Created: March 4, 2025
Last Modified: March 5, 2025
Description: Main file for reaction timing
****************************************************************************/
#include "ReactionTimer.h"
#include "LCD.h"
#include "delay.h"
#include <stdio.h>

int main(void) {
    char timeStr[16];  // Buffer for formatting time string
    
    // Initialize modules
    LCD_init();              // Initialize LCD using your existing function
    ReactionTimer_Init();    // Initialize reaction timer (now includes LED initialization)
    
    // Welcome message
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Str("Reaction Timer");
    LCD_SetCursor(1, 0);
    LCD_Str("Press SW2 start");
    
    while (1) {
        uint8_t status = ReactionTimer_Status();  // Changed from ReactionTime_Status
        
        // Check if a measurement is complete
        if (status == 2) {
            // Get reaction time and display it
            uint32_t reactionTime = ReactionTime_Get();
            
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str("Reaction Time:");
            LCD_SetCursor(1, 0);
            
            // Format and display reaction time in xxxx ms format
            // Using sprintf to format the time value with leading spaces
            sprintf(timeStr, "%4d ms", reactionTime);
            LCD_Str(timeStr);
            
            // Wait for user to see the result
            delayMs(3000);     // 3 second delay
            
            // Prompt for a new measurement
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str("Press SW2 to");
            LCD_SetCursor(1, 0);
            LCD_Str("start again");
            
            // Reset the reaction timer for a new measurement
            ReactionTimer_Reset();  // Changed from ReactionTime_Reset
        }
        
        // If timer is waiting for second button press, update display
        else if (status == 1) {  // LED check removed since it's handled by timer state
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_Str("LED is ON");
            LCD_SetCursor(1, 0);
            LCD_Str("Press SW1 now!");
            
            // Wait a bit to avoid repeated display updates
            delayMs(100);
        }
        
        // Small delay to reduce CPU load
        delayMs(50);
    }
}