#ifndef __DJI2006_H__
#define __DJI2006_H__

#include "global_can.h"
#include "main.h"

#define MOTOR_2006_CIRCLE_ANGLE 294876

//2006电机参数
typedef struct Motor_2006Type
{
	uint32_t 	ID;           //CAN ID
	uint8_t 	HCAN;         //CAN1 or CAN2
	int16_t	 	speed_rpm;    //转子转速
	int16_t	 	speed_rpm_target;
	int16_t  	current_ref;  //返回电流
	int16_t		current_set;	//设定电流
	float     tempure;      //温度
	int16_t 	angle;				//机械角度
	int32_t		total_angle;
	uint16_t 	last_angle;
	uint32_t	systime;			//本次接收的系统时间
	uint16_t RX_Frequancy;  //接收频率
} Motor_2006Type;

extern Motor_2006Type Motor_2006[1];

void Motor_2006_receive(Motor_2006Type* motor, uint8_t* temp, uint8_t CAN_ID);
void Motor_2006_send(CAN_HandleTypeDef* hcan, uint32_t StdID, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#endif
