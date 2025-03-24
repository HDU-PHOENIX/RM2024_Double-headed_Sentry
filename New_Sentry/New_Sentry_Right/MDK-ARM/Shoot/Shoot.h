#ifndef __SHOOT_H__
#define __SHOOT_H__

#include "DJI2006.h"
#include "DJI3508.h"
#include "main.h"


typedef struct Shoot_Type
{
	float Friction_Motor_Speed_Set;		//两边摩擦轮设置的弹速
	float set_bullet_speed;						//目标弹速
	uint8_t	shoot_mode;								//0：单发模式		1：连发模式
	uint8_t	use_vision;								//使用视觉
	uint8_t use_vision_has_shoot;			//视觉模式下是否已打弹
	uint8_t vision_mode;							//0:打符				1：自瞄
}Shoot_Type;


extern Shoot_Type Shoot_Data;

#define SHOOT_HEAT_ENOUGH -5500.0f


#endif


