/****************************************************************************
Author: Gabe DiMartino
Lab: Stepper Motor Control
Date Created: April 22, 2025
Description: FSM-based main program to test stepper motor movement
****************************************************************************/

#include "stepper.h"
#include "delay.h"
#include "Keypad.h"
#include "LCD.h"

typedef enum {
    STATE_WAIT_DISTANCE,
    STATE_WAIT_DIRECTION,
    STATE_MOVE
} FSMState;

int main(void) {
    Stepper_Init();
    MatrixKeypad_Init();
    LCD_Init();

    FSMState currentState = STATE_WAIT_DISTANCE;
    uint8_t distance = 0;
    int8_t direction = 1;

    while (1) {
        switch (currentState) {
            case STATE_WAIT_DISTANCE:
                LCD_Clear();
                LCD_Str("Enter distance:");
                while (1) {
                    char key = MatrixKeypad_GetKey();
                    if (key >= '0' && key <= '9') {
                        distance = key - '0';
                        currentState = STATE_WAIT_DIRECTION;
                        break;
                    }
                }
                break;

            case STATE_WAIT_DIRECTION:
                LCD_Clear();
                LCD_Str("Dir: * = CW  # = CCW");
                while (1) {
                    char dirKey = MatrixKeypad_GetKey();
                    if (dirKey == 'E') {
                        direction = 1;
                        currentState = STATE_MOVE;
                        break;
                    } else if (dirKey == 'F') {
                        direction = -1;
                        currentState = STATE_MOVE;
                        break;
                    }
                }
                break;

            case STATE_MOVE:
                LCD_Clear();
                LCD_Str("Moving ");
                LCD_data(direction == 1 ? '+' : '-');
                LCD_data(distance + '0'); // Show distance

                Stepper_Seek(distance, 5, direction); // 5 ms delay per step
                delayMs(1000);
                currentState = STATE_WAIT_DISTANCE;
                break;
        }
    }
}
