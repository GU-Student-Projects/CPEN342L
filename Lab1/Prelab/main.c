/****************************************************************************
Author: Gabe DiMartino
Lab 1: GPIO and C Language
Date Created: January 21, 2025
Last Modified: January 21, 2025
Description: this program will cycle RGB lights
Inputs: None
Outputs: RGB LED
****************************************************************************/

#include "TM4C123GH6PM.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGCGPIO_R      (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_PRGPIO_R        (*((volatile unsigned long *)0x400FEA08))

#define RED_LED   0x02
#define BLUE_LED  0x04
#define GREEN_LED 0x08

void PortF_Init(void);
void Delay(void);

int main(void){
    SYSCTL_RCC_R &= ~0x00400000;
    
    PortF_Init();
    
    while(1){
        GPIO_PORTF_DATA_R |= RED_LED;
        Delay();
        
        GPIO_PORTF_DATA_R &= ~RED_LED;
        GPIO_PORTF_DATA_R |= GREEN_LED;
        Delay();
        
        GPIO_PORTF_DATA_R &= ~GREEN_LED;
        GPIO_PORTF_DATA_R |= BLUE_LED;
        Delay();
        
        GPIO_PORTF_DATA_R &= ~BLUE_LED;
        Delay();
    }
}

void PortF_Init(void){ 
    volatile unsigned long delay;
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0){};
    
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_AMSEL_R = 0x00;
    GPIO_PORTF_PCTL_R = 0x00000000;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_AFSEL_R = 0x00;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;
}

void Delay(void){
    unsigned long volatile time;
    time = 1600000;
    while(time){
        time--;
    }
}