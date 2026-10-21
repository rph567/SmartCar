/*
* SpeedControl.c
* Created on: 2024年10月11日
* Author: 刘沐
*/
#include "zf_common_headfile.h"
#define SPEED_DIFF_GAIN     20
#define SPEED_DIFF_MAX      30
#define SPEED_MIN_FORWARD   20

int expSpeedL=0,expSpeedR=0,expSpeed=160;
int RKliff=0,LKliff=0;//差速量，单位cm/s
int MaxPWM=10000;
float speedDuty_left=0,speedDuty_right=0;
float leftspeedError=0,leftspeedError_pre=0,leftspeedError_last=0;
float rightspeedError=0,rightspeedError_pre=0,rightspeedError_last=0;
int speedP=2700,speedI=520,speedD=0;
//电机占空比控制
void Motor_PWM_Init(void)
{
    pwm_init(LeftPWM, MOTOR_FREQUENCE, 0);
    pwm_init(RightPWM, MOTOR_FREQUENCE, 0);
    gpio_init(LeftDIR, GPO, 0, GPO_PUSH_PULL);
    gpio_init(RightDIR, GPO, 0, GPO_PUSH_PULL);
}

void Set_Motor_duty(int leftduty,int rightduty)//设置电机pwm输出
{
    if(leftduty>0)
    {
        gpio_set_level(LeftDIR,0);
        pwm_set_duty(LeftPWM,leftduty);
    }
    else
    {
        gpio_set_level(LeftDIR,1);
        pwm_set_duty(LeftPWM,-leftduty);
    }
    if(rightduty>0)
    {
        gpio_set_level(RightDIR,1);
        pwm_set_duty(RightPWM,rightduty);
    }
    else
    {
        gpio_set_level(RightDIR,0);
        pwm_set_duty(RightPWM,-rightduty);
    }
}

//编码器
int16 leftPulse,rightPulse;
float leftSpeed,rightSpeed;
void Encoder_Init(void)
{
    encoder_quad_init(QTIMER1_ENCODER1, QTIMER1_ENCODER1_CH1_C0, QTIMER1_ENCODER1_CH2_C1);
    encoder_quad_init(QTIMER1_ENCODER2, QTIMER1_ENCODER2_CH1_C2, QTIMER1_ENCODER2_CH2_C24);
}

void Encoder_clear()
{
    encoder_clear_count(QTIMER1_ENCODER1);
    encoder_clear_count(QTIMER1_ENCODER2);
}

void Encoder_getvalue()
{
    leftPulse = -encoder_get_count(QTIMER1_ENCODER1);
    rightPulse = encoder_get_count(QTIMER1_ENCODER2);
}

void GetSpeed()
{
    Encoder_getvalue();
    leftSpeed  = (leftPulse*100)/(LEFT_PULSE_PER_METER*dt);
    rightSpeed = (rightPulse*100)/(RIGHT_PULSE_PER_METER*dt); 
    Encoder_clear();
}

void Speed_Correct()
{
    int diff = (int)(directionError * SPEED_DIFF_GAIN);

    diff = LimitBoth_int(diff, -SPEED_DIFF_MAX, SPEED_DIFF_MAX);
    LKliff = -diff;
    RKliff = diff;

    expSpeedL=expSpeed+LKliff;
    expSpeedR=expSpeed+RKliff;

    if(expSpeedL < SPEED_MIN_FORWARD)
        expSpeedL = SPEED_MIN_FORWARD;
    if(expSpeedR < SPEED_MIN_FORWARD)
        expSpeedR = SPEED_MIN_FORWARD;

    if(StopFlag)//停车标志位
    {
        expSpeedL=0;
        expSpeedR=0;
    }
}

//闭环速度控制
void SpeedControl()
{
    GetSpeed();
    double SPEED_CONTROL_P = speedP / 100.0;     //速度控制参数P
    double SPEED_CONTROL_D = speedD / 100.0;     //控制加速过程的限制作用P
    double SPEED_CONTROL_I = speedI / 100.0;    //速度控制参数D

    leftspeedError = expSpeedL - leftSpeed;//第三次误差           //左边速度偏差  =  期望速度-当前速度
    speedDuty_left+= SPEED_CONTROL_P * (leftspeedError-leftspeedError_last)+SPEED_CONTROL_I * leftspeedError+ speedD * (leftspeedError-2 * leftspeedError_last+leftspeedError_pre);
    leftspeedError_pre = leftspeedError_last;//第一次误差
    leftspeedError_last = leftspeedError;//第二次误差

    if (speedDuty_left > MaxPWM)                   //限幅并改变变量为最大值，否则会影响增量式PID控制
    {
        speedDuty_left = MaxPWM;
    }
    else if (speedDuty_left < -MaxPWM)
    {
        speedDuty_left = -MaxPWM;
    }

    rightspeedError = expSpeedR - rightSpeed;
    speedDuty_right+= SPEED_CONTROL_P * (rightspeedError-rightspeedError_last)+SPEED_CONTROL_I * rightspeedError  + speedD * (rightspeedError-2 * rightspeedError_last+rightspeedError_pre);
    rightspeedError_pre = rightspeedError_last;
    rightspeedError_last = rightspeedError;

        
    if (speedDuty_right > MaxPWM) //限幅并改变变量为最大值，否则会影响增量式PID控制
    {
        speedDuty_right = MaxPWM;
    }
    else if (speedDuty_right < -MaxPWM)
    {
        speedDuty_right = -MaxPWM;
    }
    
    float Duty_left = speedDuty_left*2.0/3.0;
    float Duty_right = speedDuty_right*2.0/3.0;

    Set_Motor_duty((int32)(Duty_left),(int32)(Duty_right));
}
