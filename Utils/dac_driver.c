#include "dac_driver.h"

void SSI1DAC_Init(void) {
    SYSCTL->RCGCSSI |= 2; /* enable clock to SSI1 */
    SYSCTL->RCGCGPIO |= 0x08; /* enable clock to GPIOD for SSI1*/
    while ((SYSCTL->PRGPIO & 0x08) == 0) {}; // wait for ready

    GPIOD->DEN |= 0x0B; /* PD0,1,3 as digital */
    GPIOD->AFSEL |= 0x09; /* PD0,3 alternate function */
    GPIOD->PCTL &= ~0x0000F00F;
    GPIOD->PCTL |= 0x00002002; /* Assign SSI1 */
    GPIOD->DIR |= 0x02; // PD1 as output for CS

    SSI1->CR1 = 0; /* disable SSI and configure as master */
    SSI1->CPSR = 10; /* prescaler */
    SSI1->CR0 = 0x0047; /* 16-bit data, SPI mode, 5 MHz clock */
    SSI1->CR1 |= 2; /* enable SSI1 */
}


void SSI1DAC_Write(unsigned char data) {
	while((SSI1->SR & 2) == 0) {}; /* wait until FIFO not full */
	SSI1->DR = data; /* transmit high byte */
	while(SSI1->SR & 0x10); /* wait until transmit complete */
}

void DAC_WriteVoltage(uint16_t voltage) {
    uint8_t highByte = (0x4 << 4) | ((voltage >> 8) & 0x0F);  // Control + upper DAC bits
    uint8_t lowByte = voltage & 0xFF;  // Lower DAC bits

    GPIOD->DATA &= ~0x02;  // CS Low
    SSI1DAC_Write(highByte);
    SSI1DAC_Write(lowByte);
    GPIOD->DATA |= 0x02;   // CS High
}
