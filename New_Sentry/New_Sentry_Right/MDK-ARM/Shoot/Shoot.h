#ifndef __SHOOT_H__
#define __SHOOT_H__

#include "DJI2006.h"
#include "DJI3508.h"
#include "main.h"


typedef struct Shoot_Type
{
	float Friction_Motor_Speed_Set;		//����Ħ�������õĵ���
	float set_bullet_speed;						//Ŀ�굯��
	uint8_t	shoot_mode;								//0������ģʽ		1������ģʽ
	uint8_t	use_vision;								//ʹ���Ӿ�
	uint8_t use_vision_has_shoot;			//�Ӿ�ģʽ���Ƿ��Ѵ�
	uint8_t vision_mode;							//0:���				1������
}Shoot_Type;


extern Shoot_Type Shoot_Data;

#define SHOOT_HEAT_ENOUGH -5500.0f


#endif


