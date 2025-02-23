/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: SSI implementation
****************************************************************************/

#include "ssi_driver.h"
#include "tm4c123gh6pm.h" 

void SSI_Init(void) {
    // Enable Clocks for SSI and GPIO Ports
    SYSCTL_RCGCSSI_R |= (1U << 0);  // Enable SSI0
    SYSCTL_RCGCGPIO_R |= (1U << 3) | (1U << 4);  // Enable Port D & Port E

    // Configure SSI0 Pins (PD0 = SSI0Clk, PD1 = SSI0Fss, PD2 = SSI0Rx, PD3 = SSI0Tx)
    GPIOD_AHB_AFSEL_R |= 0x0F;  
    GPIOD_AHB_PCTL_R = (GPIOD_AHB_PCTL_R & 0xFFFF0000) | 0x00001111;
    GPIOD_AHB_DEN_R |= 0x0F;
    
    // Configure PE3 (Load for 74HC165) as output
    GPIOE_AHB_DIR_R |= PORTG_LOAD_PIN;
    GPIOE_AHB_DEN_R |= PORTG_LOAD_PIN;

    // Configure PD1 (Latch for 74HC595) as output
    GPIOD_AHB_DIR_R |= PORTH_LATCH_PIN;
    GPIOD_AHB_DEN_R |= PORTH_LATCH_PIN;

    // Disable SSI before configuration
    SSI0_CR1_R = 0x00000000;

    // Set SSI clock source to system clock
    SSI0_CC_R = 0x0;

    // Configure SSI for SPI Mode 0 (CPOL = 0, CPHA = 0)
    SSI0_CPSR_R = 10; // Set clock divisor for 5MHz
    SSI0_CR0_R = 0x07; // 8-bit data, Freescale SPI Frame Format

    // Enable SSI0
    SSI0_CR1_R |= 0x02;
}

uint8_t SSI_Read_PortG(void) {
    uint8_t data;

    // Load data from switches into 74HC165
    GPIOE_AHB_DATA_R &= ~PORTG_LOAD_PIN; // Pull PE3 LOW
    for (volatile int i = 0; i < 10; i++); // Delay
    GPIOE_AHB_DATA_R |= PORTG_LOAD_PIN;  // Pull PE3 HIGH

    // Read from SSI (74HC165 shifts data out)
    while ((SSI0_SR_R & 0x04) == 0); // Wait until receive FIFO is not empty
    data = SSI0_DR_R;  // Read received data

    return data;
}

void SSI_Write_PortH(uint8_t data) {
    // Send data to 74HC595 via SSI
    while ((SSI0_SR_R & 0x02) == 0); // Wait until transmit FIFO is not full
    SSI0_DR_R = data;

    // Latch output to 74HC595
    GPIOE_AHB_DATA_R |= PORTH_LATCH_PIN; // Pull PD1 HIGH
    for (volatile int i = 0; i < 10; i++); // Small delay
    GPIOE_AHB_DATA_R &= ~PORTH_LATCH_PIN; // Pull PD1 LOW
}
