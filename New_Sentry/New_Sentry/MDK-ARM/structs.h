#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "arm_math.h"

typedef enum									//����  or  ���
{
    VISION_AUTO_AIM  =  0,
	  VISION_BUFF =  1,
}eVisionMode;

//���������ݽṹ��
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
