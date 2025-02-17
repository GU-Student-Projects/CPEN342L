#include "TrafficLight.h"

int main(void) {
    // Initialize PLL for 10MHz operation
    PLL_Init();
    
    // Initialize traffic light system
    TrafficLight_Init();
    
    // Main control loop
    while(1) {
        TrafficLight_Update();
    }
}