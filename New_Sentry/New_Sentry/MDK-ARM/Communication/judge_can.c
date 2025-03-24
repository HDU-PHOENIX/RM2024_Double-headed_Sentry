//#include "judge_can.h"
//#include "main.h"

//void set_judge_data(CAN_HandleTypeDef* hcan,uint16_t cmd ,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){
//	CAN_TxHeaderTypeDef _TxHeader;
//	uint8_t Txtemp[8];
//	_TxHeader.StdId = cmd;
//	_TxHeader.IDE = CAN_ID_STD;
//	_TxHeader.RTR = CAN_RTR_DATA;
//	_TxHeader.DLC = 0x08;
//	Txtemp[0] = (iq1 >> 8);
//	Txtemp[1] = iq1;
//	Txtemp[2] = (iq2 >> 8);
//	Txtemp[3] = iq2;
//	Txtemp[4] = iq3 >> 8;
//	Txtemp[5] = iq3;
//	Txtemp[6] = iq4 >> 8;
//	Txtemp[7] = iq4;
//	while( HAL_CAN_GetTxMailboxesFreeLevel( hcan ) == 0);
//	if(HAL_CAN_AddTxMessage(hcan,&_TxHeader,Txtemp,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
//	  {
//	  }
//}


