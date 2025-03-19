/****************************************************************************
Author: Gabe DiMartino
Lab: PWM and DC Motor
Date Created: March 18, 2025
Last Modified: March 18, 2025
Description: Implementation of motor control
****************************************************************************/

#include "motorControl.h"

int speed = 0, dir = 0;

void PWM16_Init(uint16_t period, uint16_t duty)
{
    SYSCTL->RCGCPWM |= 0x02;    // 1) Activate PWM1
    SYSCTL->RCGCGPIO |= 0x20;   // 2) Activate Port F
    for (volatile int i = 0; i < 1000; i++); // Delay for PWM to start

    GPIOF->AFSEL |= 0x04;       // Enable alt funct on PF2
    GPIOF->PCTL &= ~0x00000F00; // Configure PF2 as PWM1
    GPIOF->PCTL |= 0x00000500;
    GPIOF->AMSEL &= ~0x04;      // Disable analog functionality on PF2
    GPIOF->DEN |= 0x04;         // Enable digital I/O on PF2

    SYSCTL->RCC = 0x00100000 | (SYSCTL->RCC & (~0x000E0000)); // Configure for /2 PWM divider
    PWM1->_3_CTL = 0;           // Reloading down-counting mode 
    PWM1->_3_GENA = 0xC8;       // Low on LOAD, high on CMPA down
    PWM1->_3_LOAD = period - 1; // Cycles needed to count down to 0 
    PWM1->_3_CMPA = duty - 1;   // Count value when output rises
    PWM1->_3_CTL |= 0x00000001; // Start PWM1
    PWM1->ENABLE |= 0x00000040; // Enable PF2/M1PWM6
}

void PortB_Init(void)
{
    SYSCTL->RCGCGPIO |= 0x02;
    while ((SYSCTL->PRGPIO & 0x02) == 0) {};
    
    GPIOB->DIR |= 0x03;
    GPIOB->DEN |= 0x03;
    GPIOB->AFSEL &= ~0x03;
    GPIOB->DATA &= ~0x03;
}

void Update_LCD()
{
    LCD_Clear();

    if (dir == -1)
        LCD_data('-');
    else if (dir == 1)
        LCD_data('+');
    else
        LCD_data(' '); // Neutral state

    LCD_data((char)(speed + '0'));
}


void Set_Fwd(void)
{
    GPIOB->DATA &= ~0x03;
    GPIOB->DATA |= 0x02;
    dir = 1;
    Update_LCD();
}

void Set_Rev(void)
{
    GPIOB->DATA &= ~0x03;
    GPIOB->DATA |= 0x01;
    dir = -1;
    Update_LCD();
}

void Set_Speed(int new_speed)
{
    if (new_speed > 9) return;

    speed = new_speed;
    
    if (speed == 0)
    {
        dir = 0;
        GPIOB->DATA &= ~0x03;
    }
    else if (dir == 0)
    {
        Set_Fwd();
    }

    PWM1->_3_CMPA = (5000 / 9 * new_speed) - 1;
    Update_LCD();
}

void Motor_Init(uint16_t period)
{
    PWM16_Init(period, period / 2);
    PortB_Init();
    Set_Speed(0);
}
