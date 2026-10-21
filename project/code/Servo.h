#include "zf_common_typedef.h"
/*
 * Servo.c
 * Created on: 2024年10月11日
 * Author: 刘沐
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#define SERVO_PWM PWM4_MODULE2_CHA_C30

void Servo_PWM_Init(void);
void Get_Direction_Error();
void Direction_Control();
void Direction_Control_Error();
extern int ServoMid;
extern int ServoControlOut;
extern int direction_Kp;
extern int direction_Kd;
extern int direction_Ki;
extern float directionError;
extern int directionValid;

#endif /* CODE_SERVO_H_ */
