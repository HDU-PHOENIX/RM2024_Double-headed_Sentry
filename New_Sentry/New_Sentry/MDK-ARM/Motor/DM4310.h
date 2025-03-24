#ifndef __DM4310_H__
#define __DM4310_H__

#include "global_can.h"
#include "main.h"

typedef struct Motor_4310Type
{
	uint32_t 	ID;           //CAN ID
	uint8_t 	HCAN;         //CAN1 or CAN2
	float	 	speed_rpm;    //转子转速
	float	 	speed_rpm_target;
	int16_t  	current_ref;  //返回电流
	int16_t		current_set;	//设定电流
	float     tempure;      //温度
	float 	angle;				//机械角度
	float   position;     //编码器
	int32_t		total_angle;
	uint16_t 	last_angle;
	uint32_t	systime;			//本次接收的系统时间
	uint16_t  dm_err;       //判断帧，0失能，1使能
	uint8_t  	hall;
	
	uint32_t msg_missing_time;
	uint32_t msg_cnt;
	uint32_t msg_frequent; //接收频率
} Motor_4310Type;

extern Motor_4310Type Motor_4310;

void Motor_4310_start(CAN_HandleTypeDef* hcan,uint16_t id);
void Motor_4310_send(CAN_HandleTypeDef* hcan,uint16_t id, float _pos, float _vel, float _KP, float _KD, float _torq);
void Motor_4310_receive(Motor_4310Type* motor, uint8_t* temp, uint8_t CAN_ID);

#endif
