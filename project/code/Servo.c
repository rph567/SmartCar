/*
 * Servo.c
 * Created on: 2024年10月11日
 * Author: 刘沐
 */
#include "zf_common_headfile.h"

#define DIRECTION_NEAR_OFFSET      10
#define DIRECTION_FAR_OFFSET       15
#define DIRECTION_TRACK_MIN_WIDTH  10

int ServoMid=1035,ServoRMax=120,ServoLMax=120;    //需用舵机测试

float directionError = 0;
int direction_Kp=130,direction_Kd=80,direction_Ki=10;//舵机PD,循迹主要需要调的参数
int ServoControlOut=0;//舵机实际输出值
int directionValid=0;//三行误差是否有效

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
    static float lastDirectionError = 0.0f;
    static int hasLastDirectionError = 0;

    int rows[3];
    float rowErrors[3] = {0.0f, 0.0f, 0.0f};
    float rowWeights[3] = {0.25f, 0.50f, 0.25f};  //此处可以调节三行控制权重
    int rowValid[3] = {0, 0, 0};
    float weightedError = 0.0f;
    float weightSum = 0.0f;
    int minRow = EndRow + 2;
    int i, j;

    if(minRow >= ROW)
    {
        directionValid = 0;
        directionError = hasLastDirectionError ? lastDirectionError : 0.0f;
        return;
    }

    if(minRow < 0)
    {
        minRow = 0;
    }

    //行号越大越靠近车辆，近处权重最高，远处只提供提前量
    rows[0] = LimitBoth_int(ControlRow + DIRECTION_NEAR_OFFSET, minRow, ROW - 1);
    rows[1] = LimitBoth_int(ControlRow, minRow, ROW - 1);
    rows[2] = LimitBoth_int(ControlRow - DIRECTION_FAR_OFFSET, minRow, ROW - 1);

    for(i = 0; i < 3; i++)
    {
        int row = rows[i];
        int left = LeftLine[row];
        int right = RightLine[row];

        rowValid[i] = (left >= 0) &&
                      (right < COL) &&
                      (left < right) &&
                      ((right - left) >= DIRECTION_TRACK_MIN_WIDTH) &&
                      (MidLine[row] >= 0) &&
                      (MidLine[row] < COL);

        //行号被边界限幅后可能重复，只保留权重更高的近处行
        for(j = 0; j < i; j++)
        {
            if(rows[j] == row)
            {
                rowValid[i] = 0;
            }
        }

        if(rowValid[i])
        {
            rowErrors[i] = (float)(MidLine[row] - (COL / 2));
            weightedError += rowWeights[i] * rowErrors[i];
            weightSum += rowWeights[i];
        }
    }

    directionValid = (weightSum > 0.0f);

    if(directionValid)
    {
        weightedError /= weightSum;
        directionError = -3.0f * weightedError / COL;
        directionError = LimitBoth_float(directionError, -1.0f, 1.0f);
        lastDirectionError = directionError;
        hasLastDirectionError = 1;
    }
    else
    {
        //三行都无效时保持上一帧有效误差，避免瞬时丢线把舵机拉回中位
        directionError = hasLastDirectionError ? lastDirectionError : 0.0f;
    }
}

void Direction_Control_Error(void)
{
    static float error1 = 0, error2 = 0;   // error1: 本次误差, error2: 上次误差
    float d_error;
    
    

    /*
     刘沐：简单写个舵机PID来位置闭环，这个舵机PID将是校内赛你们需要花最多时间进行调试的地方，加油捏
    */
    // 注意：directionError 在 main 里已经由 Get_Direction_Error() 算好，这里直接用

    error1  = directionError;               // 本次误差
    d_error = error1 - error2;              // 误差变化量(微分项)

    // 舵机 PD 输出：中位 + 比例项 + 微分项
    ServoControlOut = ServoMid
                    + direction_Kp * error1
                    + direction_Kd * d_error;

    ServoControlOut = LimitBoth_int(ServoControlOut,ServoMid - ServoRMax,ServoMid + ServoLMax);//舵机输出限幅

    error2 = error1;                        // 保存本次误差，供下次算微分

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



