#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include "main.h"

#define CHASSIS_DECELE_RATIO  19		//���ٱ�
#define LENGTH_A 200         //mm
#define LENGTH_B 200         //mm
#define WHEEL_PERIMETER 152  //mm//�ܳ�

typedef struct Chassis_Type
{
	float vx_target;
	float vy_target;
	float vw_target;
	float motor1_speed_target;
	float motor2_speed_target;
	float motor3_speed_target;
	float motor4_speed_target;
	float angle_yaw_error;		//�������������̨������֮������Ƕ�
}Chassis_Type;

extern Chassis_Type Chassis_Data;


#endif

