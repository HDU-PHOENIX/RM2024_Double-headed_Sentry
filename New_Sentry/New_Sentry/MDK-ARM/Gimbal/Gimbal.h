#ifndef __GIMBAL_H__
#define __GIMBAL_H__

#include "main.h"

typedef struct Gimbal_Type
{

	uint8_t  HCAN;
	
	float  Speed_Vx;
	float  Speed_Vy;
	float  yaw_angle;
	float  yaw_angle_target;
	float  imu;
	
}Gimbal_Type;

extern Gimbal_Type Gimbal_Data;

#endif
