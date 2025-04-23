#ifndef STEPPER_H_
#define STEPPER_H_

#include <stdint.h>

void Stepper_Init(void);
void Stepper_Step(int steps);
void Stepper_SetDirection(int direction); // 1 for CW, -1 for CCW

#endif /* STEPPER_H_ */
