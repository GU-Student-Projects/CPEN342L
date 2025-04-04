#include "adc_driver.h"

void ADC1_InitCh2(void) {  

volatile uint32_t delay; 

    SYSCTL->RCGCADC |= 0x02;  // 1) Activate ADC1 clock 
    SYSCTL->RCGCGPIO |= 0x10; // 1) Activate clock for Port E 
    delay = SYSCTL->RCGCGPIO; // Allow time for clock to stabilize 
    
    GPIOE->DIR &= ~0x02;   // Make PE1 an input 
    GPIOE->AFSEL |= 0x02;  // Enable alternate function on PE1 
    GPIOE->DEN &= ~0x02;   // Disable digital function on PE1 
    GPIOE->AMSEL |= 0x02;  // Enable analog functionality on PE1 
    
    ADC1->PC = 0x01;       // 2) Set ADC sample rate to 125K SPS 
    ADC1->SSPRI = 0x3210;  // 3) Set sequencer 3 as lowest priority 
    ADC1->ACTSS &= ~0x08;  // 4) Disable SS3 before configuration 
    ADC1->EMUX &= ~0xF000; // 5) Software trigger conversion for SS3 
    ADC1->SSMUX3 = 2;      // 6) Select AIN2 (PE1) 
    ADC1->SSCTL3 = 0x06;   // 7) Set IE0 and END0 
    ADC1->IM &= ~0x08;     // 8) Disable SS3 interrupts 
    ADC1->ACTSS |= 0x08;   // 9) Enable SS3 
  

} 

 

uint16_t ADC1_InCh2(void) {  

    ADC1->PSSI = 0x08; // Start conversion on SS3  

    while ((ADC1->RIS & 0x08) == 0); // Wait for conversion to complete  

    uint16_t result = ADC1->SSFIFO3 & 0xFFF; // Read 12-bit result  

    ADC1->ISC = 0x08; // Clear completion flag  

return result;  

} 