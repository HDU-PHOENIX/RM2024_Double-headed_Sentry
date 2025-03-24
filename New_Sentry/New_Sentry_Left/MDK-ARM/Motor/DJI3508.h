#ifndef __DJI3508_H__
#define __DJI3508_H__

#include "global_can.h"
#include "main.h"

//3508�������
typedef struct Motor_3508Type
{
	uint32_t 	ID;           //CAN ID
	uint8_t 	HCAN;         //CAN1 or CAN2
	int16_t	 	speed_rpm;    //ת��ת��
	int16_t	 	speed_rpm_target;
	int16_t  	current_ref;  //���ص���
	int16_t		current_set;	//�趨����
	float     tempure;      //�¶�
	int16_t 	angle;				//��е�Ƕ�
	uint32_t	systime;			//���ν��յ�ϵͳʱ��
	uint16_t RX_Frequancy;  //����Ƶ��
} Motor_3508Type;

extern Motor_3508Type Motor_3508[2];

void Motor_3508_receive(Motor_3508Type* motor, uint8_t* temp, uint8_t CAN_ID);
void Motor_3508_send(CAN_HandleTypeDef* hcan, uint32_t StdID, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#endif
