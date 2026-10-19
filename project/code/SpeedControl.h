/*
 * SpeedControl.h
 * Created on: 2024年10月11日
 * Author: 刘沐
 */
#include "zf_common_typedef.h"

#ifndef CODE_SPEEDCONTROL_H_
#define CODE_SPEEDCONTROL_H_

//电机占空比
#define LeftDIR         D1
#define LeftPWM         PWM1_MODULE3_CHA_D0
#define RightDIR        D2
#define RightPWM        PWM2_MODULE3_CHB_D3  

#define MOTOR_FREQUENCE 17000
extern int FrictionCoefficient,Acceleration,acceleration;
void Motor_PWM_Init(void);
void Set_Motor_duty(int leftduty,int rightduty);

//编码器
#define  dt 0.002 //  采样时间(即中断时间）0.002s=2ms
#define  RIGHT_PULSE_PER_METER 12000 //-5720
#define  LEFT_PULSE_PER_METER 12000 //5700
extern int16 leftPulse,rightPulse;
extern float leftSpeed,rightSpeed;
extern int expSpeedL,expSpeedR;
extern int RKliff,LKliff;
extern int expSpeed;


void Encoder_Init(void);
void Encoder_clear();
void Encoder_getvalue();
void GetSpeed();
void Speed_Correct();
//闭环速度控制
void SpeedControl();


#endif /* CODE_SPEEDCONTROL_H_ */
