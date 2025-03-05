#include "TrafficLight.h"

static const uint32_t STATE_DELAYS[] = {10, 2, 1, 10, 2, 1};  // Delays in seconds
static volatile uint32_t SysTick_Count = 0;
static volatile TrafficState currentState = NS_GREEN_EW_RED;

void PLL_Init(void) {
    // Configure for 10 MHz clock
    SYSCTL->RCC2 |= 0x80000000;  // Use RCC2
    SYSCTL->RCC2 |= 0x00000800;  // Bypass PLL during config
    SYSCTL->RCC = (SYSCTL->RCC & ~0x000007C0) + 0x00000540;  // 10 MHz crystal
    SYSCTL->RCC2 &= ~0x00000070;  // Configure for main oscillator source
    SYSCTL->RCC2 &= ~0x00002000;  // Activate PLL by clearing PWRDN
    SYSCTL->RCC2 |= 0x40000000;   // Use 400 MHz PLL
    SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (4 << 22);  // Divisor = 4 for 10 MHz
    while((SYSCTL->RIS & 0x00000040) == 0) {};  // Wait for PLL lock
    SYSCTL->RCC2 &= ~0x00000800;  // Enable PLL by clearing bypass
}

static void PortB_Init(void) {
    volatile uint32_t delay;
    SYSCTL->RCGCGPIO |= 0x02;     // Enable Port B clock
    delay = SYSCTL->RCGCGPIO;      // Allow time for clock to start
    GPIOB->DIR |= 0x3F;           // Make PB5-0 outputs
    GPIOB->DEN |= 0x3F;           // Enable digital I/O on PB5-0
    GPIOB->DATA = 0;              // Initialize all lights to off
}

static void SysTick_Init(void) {
    SysTick->CTRL = 0;                  // Disable SysTick during setup
    SysTick->LOAD = 10000000 - 1;       // 1-second intervals with 10MHz clock
    SysTick->VAL = 0;                   // Any write to current clears it
    SysTick->CTRL = 0x07;               // Enable with interrupts
}

static void SetTrafficLights(TrafficState state) {
    uint32_t light_pattern = 0;

    switch(state) {
        case NS_GREEN_EW_RED:
            light_pattern = (1 << 0) | (1 << 5);  // PB0=1, PB5=1
            break;
        case NS_YEL_EW_RED:
            light_pattern = (1 << 1) | (1 << 5);  // PB1=1, PB5=1
            break;
        case NS_RED_EW_RED_1:
            light_pattern = (1 << 2) | (1 << 5);  // PB2=1, PB5=1
            break;
        case NS_RED_EW_GREEN:
            light_pattern = (1 << 3) | (1 << 2);  // PB3=1, PB2=1
            break;
        case NS_RED_EW_YEL:
            light_pattern = (1 << 4) | (1 << 2);  // PB4=1, PB2=1
            break;
        case NS_RED_EW_RED_2:
            light_pattern = (1 << 5) | (1 << 2);  // PB5=1, PB2=1
            break;
    }

    GPIOB->DATA = ~light_pattern;  // Set only the required pins, all others are 0
}

void TrafficLight_Init(void) {
    PLL_Init();                            // Configure for 10MHz
    PortB_Init();                          // Initialize GPIO Port B
    SysTick_Init();                        // Initialize SysTick timer
    SysTick_Count = STATE_DELAYS[currentState];  // Load initial delay
    SetTrafficLights(currentState);        // Set initial light state
}

void SysTick_Handler(void) {
    if (SysTick_Count > 0) {
        SysTick_Count--;
    } else {
        // Move to next state
        currentState = (TrafficState)((currentState + 1) % 6);
        SysTick_Count = STATE_DELAYS[currentState];  // Set new delay
        SetTrafficLights(currentState);
			  delayMs(STATE_DELAYS[currentState]*1000);
    }
}
