/****************************************************************************
Author: Gabe DiMartino
Lab: IO Interfacing with SSI
Date Created: February 23, 2025
Last Modified: February 23, 2025
Description: SSI implementation
****************************************************************************/

#include "ssi_driver.h"

void SSI_Init(void) {
    SYSCTL->RCGCSSI |= 0x02;  //Enable clock for SSI1
    SYSCTL->RCGCGPIO |= 0x018; //Enable clock for Port D and E
    while ((SYSCTL->PRGPIO & 0x18) == 0) {}; //Wait for Port D, E

    GPIOD->AFSEL |= 0x0F;  //Pins 0 1 2 3 
		GPIOD->PCTL = (GPIOD->PCTL & 0xFFFF0000) | 0x00002222; //Set SSI1
    GPIOD->AMSEL &= ~0x0F; //Disable analog mode
		GPIOD->DEN |= 0x0F; //Enable digital mode
    SSI1->CR1 = 0x00;
		SSI1->CR1 = 0x00;
    SSI1->CPSR = 2; //Set clock
    SSI1->CR0 = (SSI1->CR0 & ~0xFFF0) | (4 << 8);  //Set SER for clock all else zero
		SSI1->CR0 = (SSI1->CR0 & ~0x0F) | 0x07;
    SSI1->CR1 |= 0x02;
    GPIOE->DIR |= 0x08;   // Set PE3 as output, load toggle
    GPIOE->DEN |= 0x08;  
		GPIOE->DATA=0x08; //Default state high
}

void SSI_Write(unsigned char data) {
    while ((SSI1->SR & 2) == 0) {}; //Wait until FIFO not full
    SSI1->DR = data; //Transmit a byte
    while (SSI1->SR & 0x10) {}; //Wait until transmission is complete
}

unsigned char SSI_Read(void) {
	GPIOE->DATA &= ~0x08;       // Pull load intputs into register
		volatile int delay; //Delay
    for(delay = 0; delay < 100; delay++); 
		GPIOE->DATA |= 0x08; //Back into shift mode
		while((SSI1->SR & 0x01) == 0) {};/* wait until FIFO empty */
		SSI1->DR = 0; /* trigger 8 SCK pulses to shift in data */
		while((SSI1->SR & 0x04) == 0) {}; /* wait until FIFO not empty */
		return SSI1->DR;
}


