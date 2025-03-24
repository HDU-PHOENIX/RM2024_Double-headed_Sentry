#include "global_can.h"
#include "DJI3508.h"
#include "DJI6020.h"
#include "DJI2006.h"
#include "DM4310.h"
#include "dbus_can.h"
#include "dbus.h"
#include "judge.h"
#include "vision.h"
#include "vision_can.h"

void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan)
{
	//can1 &can2 use same filter config
	CAN_FilterTypeDef		sFilterConfig;


   sFilterConfig.FilterIdHigh         = 0x0000;
   sFilterConfig.FilterIdLow          = 0x0000;
   sFilterConfig.FilterMaskIdHigh     = 0x0000;
   sFilterConfig.FilterMaskIdLow      = 0x0000;
   sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   sFilterConfig.FilterBank=0;
   sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
   sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
   sFilterConfig.FilterActivation = ENABLE;
   sFilterConfig.SlaveStartFilterBank = 0;
	if(HAL_CAN_ConfigFilter(_hcan, &sFilterConfig) != HAL_OK)
	{

	}


}

void CAN_Init(void)
{
	my_can_filter_init_recv_all(&hcan1);
	HAL_CAN_Start(&hcan1);
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	my_can_filter_init_recv_all(&hcan2);
	HAL_CAN_Start(&hcan2);
	__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* _hcan)
{
	
	CAN_RxHeaderTypeDef _RxHeader;
	uint8_t temp[8];
	HAL_CAN_GetRxMessage(_hcan,CAN_RX_FIFO0,&_RxHeader,temp);
	//接收伺服电机can信号
	switch(_RxHeader.StdId)
	{
		case CANID_GIMBAL_3508_1:		//右摩擦轮
		{
			Motor_3508_receive(&Motor_3508[0], temp, (uint8_t)CANID_GIMBAL_3508_1);
		}break;
		case CANID_GIMBAL_3508_2:		//左摩擦轮
		{
			Motor_3508_receive(&Motor_3508[1], temp, (uint8_t)CANID_GIMBAL_3508_2);
		}break;
		case CANID_GIMBAL_2006:		//拨弹盘
		{
			Motor_2006_receive(&Motor_2006[0], temp, (uint8_t)CANID_GIMBAL_2006);
		}break;
		case CANID_GIMBAL_6020:
		{
			Motor_6020_receive(&Motor_6020[0], temp, (uint8_t)CANID_GIMBAL_6020);
		}break;
		case CANID_PITCH_4310:		//小PITCH电机
		{
			Motor_4310_receive(&Motor_4310, temp, (uint8_t)CANID_PITCH_4310);
		}break;
		default:
			break;
	}
	
	

	  __HAL_CAN_ENABLE_IT(_hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}



