#ifndef __GIMBAL_H__
#define __GIMBAL_H__

#include "main.h"
#include "Shoot.h"

typedef enum
{
 ALL_POWER_OFF_MODE = 0x00,
 FOLLOW_MODE ,
 UNFOLLOW_MODE ,
 AUTO_MODE ,
 SHOOT_MODE ,
 SPINNING ,
 FIX_MODE ,
}robot_mode_t;

typedef struct RobotDataType
{
	robot_mode_t	robot_mode;	
	
}RobotDataType;

//陀螺仪数据结构体
typedef struct IMUType
{
	float ol[3];
	float accel[3];
	float quat[4];
	float gyro[3];
	float ol_accel[3];
	float temp;
	float yaw_target;
	float pitch_target;
}IMUType;


typedef struct Gimbal_Type
{
	
	uint8_t  HCAN;
	
	IMUType						IMUData;
	RobotDataType			RobotData;
	Shoot_Type        Shoot_Data;
	
	
}Gimbal_Type;

extern Gimbal_Type Gimbal_Data;

void Vision_Move(void);
void Robot_Mode_Select(void);
void Remote_Move(void);

#endif
