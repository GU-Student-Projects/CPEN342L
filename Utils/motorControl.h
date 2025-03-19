#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "LCD.h"

extern int speed, dir;

void PWM16_Init(uint16_t period, uint16_t duty);
void PortB_Init(void);
void Update_LCD(void);
void Set_Fwd(void);
void Set_Rev(void);
void Set_Speed(int new_speed);
void Motor_Init(uint16_t period);

#endif // MOTOR_CONTROL_H
