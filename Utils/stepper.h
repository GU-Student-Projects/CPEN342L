#ifndef STEPPER_H_
#define STEPPER_H_

#include <stdint.h>

void Stepper_Init(void);
void Stepper_Step(int steps);
void Stepper_SetDirection(int direction); // 1 for CW, -1 for CCW
void Stepper_Seek(uint8_t desired, uint32_t time, uint8_t direction);

#endif /* STEPPER_H_ */
