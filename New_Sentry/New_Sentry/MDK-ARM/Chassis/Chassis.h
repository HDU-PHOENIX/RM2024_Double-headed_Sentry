#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include "main.h"

#define CHASSIS_DECELE_RATIO  19		//减速比
#define LENGTH_A 220         //mm
#define LENGTH_B 220         //mm
#define WHEEL_PERIMETER 152  //mm//周长

typedef enum
{
 ALL_POWER_OFF_MODE = 0x00,
 FOLLOW_MODE ,
 UNFOLLOW_MODE ,
 AUTO_MODE ,
 SHOOT_MODE ,
 SPINNING ,
 DIFFSPINNING ,
}robot_mode_t;

typedef struct RobotDataType
{
	robot_mode_t	robot_mode;	
	
	
}RobotDataType;

typedef struct Chassis_Type
{
	float vx_target;
	float vy_target;
	float vw_target;
	RobotDataType			RobotData;
	float motor1_speed_target;
	float motor2_speed_target;
	float motor3_speed_target;
	float motor4_speed_target;
	float angle_yaw_error;		//底盘正方向和云台正方向之间的相差角度
}Chassis_Type;

extern Chassis_Type Chassis_Data;

#endif

