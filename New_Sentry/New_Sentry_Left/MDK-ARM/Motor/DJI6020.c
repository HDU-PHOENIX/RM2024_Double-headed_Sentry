#include "DJI6020.h"
#include "Kalman.h"

KalmanType KAL_PITCH_6020_SPEED;
Motor_6020Type Motor_6020[1];			//pitch轴电机为0，yaw轴电机为1

void Motor_6020_receive(Motor_6020Type* motor, uint8_t* temp, uint8_t CAN_ID)
{
	motor->ID 						= CAN_ID;
	motor->angle         	= ((uint16_t)temp[0])<<8 | ((uint16_t)temp[1]) ;
	motor->last_speed_rpm = motor->speed_rpm;
	motor->speed_rpm     	= (int16_t)(temp[2]<<8 |temp[3]);
	motor->current_ref 		= (((int16_t)temp[4])<<8 | ((int16_t)temp[5]));
	motor->tempure       	= temp[6];
	motor->last_angle     = motor->angle;
	
}

void Motor_6020_send(CAN_HandleTypeDef* hcan, uint32_t StdID, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	CAN_TxHeaderTypeDef _TxHeader;
	uint8_t Txtemp[8];
	_TxHeader.StdId = StdID;
	_TxHeader.IDE = CAN_ID_STD;
	_TxHeader.RTR = CAN_RTR_DATA;
	_TxHeader.DLC = 0x08;
	Txtemp[0] = (iq1 >> 8);
	Txtemp[1] = iq1;
	Txtemp[2] = (iq2 >> 8);
	Txtemp[3] = iq2;
	Txtemp[4] = iq3 >> 8;
	Txtemp[5] = iq3;
	Txtemp[6] = iq4 >> 8;
	Txtemp[7] = iq4;

		uint8_t count = 0;
	while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan1 ) == 0 && count < 100){
			count++;
	};
		if(HAL_CAN_AddTxMessage(hcan,&_TxHeader,Txtemp,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
	{
		
	}
}
