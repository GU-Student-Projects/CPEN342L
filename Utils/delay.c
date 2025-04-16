// delay.c
/****************************************************************************
Author: Gabe DiMartino
Lab: Keypad and LCD Interface
Date Created: January 26, 2025
Last Modified: January 26, 2025
Description: Implementation of delay function
****************************************************************************/
#include "delay.h"
/* delay n milliseconds */
void delayMs(int n) {
    int i, j;
    for(i = 0 ; i< n; i++)
        for(j = 0; j < 6265; j++)
            {} /* do nothing for 1 ms */
}

void delayUs(uint32_t us){
    SysTick->LOAD = (us * 16) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 5;

    while ((SysTick->CTRL & 0x100000) == 0);

    SysTick->CTRL = 0;
}
