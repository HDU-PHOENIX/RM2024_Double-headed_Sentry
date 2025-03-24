#include "DJI2006.h"

Motor_2006Type Motor_2006[1];		//拨弹盘

/**
*@bref 电机上电角度=0， 之后用这个函数更新3508电机的相对开机后（为0）的相对角度。
	*/
#define ABS(x)	( (x>0) ? (x) : (-x) )
void get_total_angle(Motor_2006Type *p)
{
	int res1, res2, delta;
	if(p->angle < p->last_angle)
  {			//可能的情况
		res1 = p->angle + 8192 - p->last_angle;	//正转，delta=+
		res2 = p->angle - p->last_angle;				//反转	delta=-
	}
  else
  {	//angle > last
		res1 = p->angle - 8192 - p->last_angle ;//反转	delta -
		res2 = p->angle - p->last_angle;				//正转	delta +
	}
	//不管正反转，肯定是转的角度小的那个是真的
	if(ABS(res1)<ABS(res2))
		delta = res1;
	else
		delta = res2;

	p->total_angle += delta;
	p->last_angle = p->angle;	
	
}

void Motor_2006_receive(Motor_2006Type* motor, uint8_t* temp, uint8_t CAN_ID)
{
	motor->ID 						= CAN_ID;
	motor->angle         	= ((uint16_t)temp[0])<<8 | ((uint16_t)temp[1]) ;
	motor->speed_rpm     	= (int16_t)(temp[2]<<8 |temp[3]);
	motor->current_ref 		= (((int16_t)temp[4])<<8 | ((int16_t)temp[5]));
	motor->tempure       	= temp[6];
	get_total_angle(motor);
}

void Motor_2006_send(CAN_HandleTypeDef* hcan, uint32_t StdID, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
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
