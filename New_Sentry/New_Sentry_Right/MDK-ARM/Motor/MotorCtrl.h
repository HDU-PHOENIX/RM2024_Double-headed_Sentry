#ifndef __MOTORCTRL_H__
#define __MOTORCTRL_H__

#include "DJI6020.h"
#include "DJI3508.h"
#include "DM4310.h"
#include "DJI2006.h"
#include "pid.h"
//3796
#define GIMBLE_YAW_MOTOR_ANGLE 0.1f 		//当云台正方向与底盘正方向一致时，大yaw轴电机的对应绝对角度

#define GIMBLE_INIT_YAW_OK_ANGLE 10

extern PidTypeDef PID_3508_RIGHT_SPEED;
extern PidTypeDef PID_3508_LEFT_SPEED;
extern PidTypeDef PID_2006_SHOOT_SPEED;
extern PidTypeDef PID_VISION_YAW_ANGLE;
extern PidTypeDef PID_VISION_PITCH_ANGLE;
extern float VISION_YAW_VFF,VISION_PITCH_VFF;

extern float PID_VISION_YAW_ANGLE_K_ZIMIAO[3];
extern float PID_VISION_PITCH_ANGLE_K_ZIMIAO[3];
extern float PID_VISION_YAW_ANGLE_K[3];
extern float PID_VISION_PITCH_ANGLE_K[3];

extern void Handle_Angle8192_PID_Over_Zero(float *tar, float *cur);

#endif
