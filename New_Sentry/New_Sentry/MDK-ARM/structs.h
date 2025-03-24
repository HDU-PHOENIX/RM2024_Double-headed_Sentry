#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "arm_math.h"

typedef enum									//自瞄  or  打符
{
    VISION_AUTO_AIM  =  0,
	  VISION_BUFF =  1,
}eVisionMode;

//陀螺仪数据结构体
typedef struct IMUType
{
	float ol[3];
	float accel[3];
	float quat[4];
	float gyro[3];
	float temp;
	float yaw_target;
	float pitch_target;
	float pitch_machine;
}IMUType;


#endif
