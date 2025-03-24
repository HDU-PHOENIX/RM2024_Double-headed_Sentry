//#include "dbus_can.h"
//#include "main.h"

////°å¼äÍ¨ÐÅÒ£¿ØÆ÷
//void set_rc_ch(int16_t ch1, int16_t ch2, int16_t ch3, int16_t ch4)
//{
//	CAN_TxHeaderTypeDef _TxHeader;
//	uint8_t Txtemp[8];
//	_TxHeader.StdId = CANID_CHASSIS_C;
//	_TxHeader.IDE = CAN_ID_STD;
//	_TxHeader.RTR = CAN_RTR_DATA;
//	_TxHeader.DLC = 0x08;
//	Txtemp[0] = (ch1 >> 8);
//	Txtemp[1] = ch1;
//	Txtemp[2] = (ch2 >> 8);
//	Txtemp[3] = ch2;
//	Txtemp[4] = ch3 >> 8;
//	Txtemp[5] = ch3;
//	Txtemp[6] = ch4 >> 8;
//	Txtemp[7] = ch4;
//	uint8_t count = 0;
//	while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan2 ) == 0 && count < 100){
//			count++;
//	};
//	if(HAL_CAN_AddTxMessage(&CAN_CHASSIS_C,&_TxHeader,Txtemp,(uint32_t*)CAN_TX_MAILBOX1)!=HAL_OK)
//	{
//		
//	}
//	
//}

//void set_rc_key(uint8_t sw1, uint8_t sw2, int16_t v, int16_t wheel)
//{
//	CAN_TxHeaderTypeDef _TxHeader;
//	uint8_t Txtemp[8];
//	_TxHeader.StdId = CANID_CHASSIS_C_1;
//	_TxHeader.IDE = CAN_ID_STD;
//	_TxHeader.RTR = CAN_RTR_DATA;
//	_TxHeader.DLC = 0x08;
//	Txtemp[0] = sw1;
//	Txtemp[1] = sw2;
//	Txtemp[2] = (v >> 8);
//	Txtemp[3] = v;
//	Txtemp[4] = wheel >> 8;
//	Txtemp[5] = wheel;
//	uint8_t count = 0;
//	while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan2 ) == 0 && count < 100){
//			count++;
//	};
//	if(HAL_CAN_AddTxMessage(&CAN_CHASSIS_C,&_TxHeader,Txtemp,(uint32_t*)CAN_TX_MAILBOX1)!=HAL_OK)
//	{
//		
//	}
//}
