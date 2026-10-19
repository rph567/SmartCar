/*
 * Servo.c
 * Created on: 2024年10月11日
 * Author: 刘沐
 */
#include "zf_common_headfile.h"

int ServoMid=1220,ServoRMax=120,ServoLMax=120;    //需用舵机测试

float directionError = 0;
int direction_Kp=80,direction_Kd=80,direction_Ki=10;//舵机PD,循迹主要需要调的参数
int ServoControlOut=0;//舵机实际输出值

void Servo_PWM_Init(void)
{
    pwm_init(SERVO_PWM, 75, ServoMid);
}
void servo_duty(uint32 duty)
{
    pwm_set_duty(SERVO_PWM, duty);
}
void Get_Direction_Error()
{
    //计算误差并归一化
    directionError = ((float)(MidLine[ControlRow]-(COL/2)))/COL;;

    directionError*=-3.0;//放大和限幅范围由车实际情况调整

    directionError= LimitBoth_float(directionError,-1.0,1.0);
}

void Direction_Control_Error(void)
{
    static float error1, error2;
    /*
     刘沐：简单写个舵机PID来位置闭环，这个舵机PID将是校内赛你们需要花最多时间进行调试的地方，加油捏
    */

    ServoControlOut = LimitBoth_int(ServoControlOut,ServoMid - ServoRMax,ServoMid + ServoLMax);//舵机输出限幅

    /*
     刘沐：
     下面是停车时把舵机打正，防止舵机乱晃
     不想要可以删掉只保留 pwm_set_duty(SERVO_PWM,ServoControlOut);就行
    */
    if(StopFlag)
        pwm_set_duty(SERVO_PWM,ServoMid);
    else
        pwm_set_duty(SERVO_PWM,ServoControlOut);
}



