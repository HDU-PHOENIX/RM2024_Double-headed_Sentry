#ifndef __DJI6020_H__
#define __DJI6020_H__

#include "global_can.h"
#include "main.h"
#include "Kalman.h"

//6020电机参数
typedef struct Motor_6020Type
{
	uint32_t 	ID;           //CAN ID
	uint8_t 	HCAN;         //CAN1 or CAN2
	int16_t	 	speed_rpm;    //转子转速
	int16_t   last_speed_rpm;
	int16_t   v_speed_rpm;  //滤波后速度
	int16_t		speed_rpm_target;
	int16_t  	current_ref;  //返回电流
	int16_t		current_set;	//设定电流
	float     tempure;      //温度
	int16_t 	angle;				//机械角度
	int16_t   last_angle;
	float		angle_target;	//目标角度
	uint32_t	systime;			//本次接收的系统时间
	uint16_t RX_Frequancy;  //接收频率
} Motor_6020Type;

extern Motor_6020Type Motor_6020[1];
extern KalmanType KAL_PITCH_6020_SPEED;

void Motor_6020_receive(Motor_6020Type* motor, uint8_t* temp, uint8_t CAN_ID);
void Motor_6020_send(CAN_HandleTypeDef* hcan, uint32_t StdID, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#endif
