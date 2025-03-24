//#include "dbus_can.h"

//void Dbus_receive(RC_Ctl_t* RC_Ctl_data, uint8_t* temp, uint8_t CAN_ID)
//{
//	if(CAN_ID == (uint8_t)CANID_CHASSIS_C)
//	{
//		RC_Ctl_data -> rc.ch0 = ((uint16_t)temp[0])<<8 | ((uint16_t)temp[1]) ;
//		RC_Ctl_data -> rc.ch1 = ((uint16_t)temp[2])<<8 | ((uint16_t)temp[3]) ;
//		RC_Ctl_data -> rc.ch2 = ((uint16_t)temp[4])<<8 | ((uint16_t)temp[5]) ;
//		RC_Ctl_data -> rc.ch3 = ((uint16_t)temp[6])<<8 | ((uint16_t)temp[7]) ;
//	}
//	else if(CAN_ID == (uint8_t)CANID_CHASSIS_C_1)
//	{
//		RC_Ctl_data ->rc.s1 = temp[0];
//		RC_Ctl_data ->rc.s2 = temp[1];

//		RC_Ctl_data ->rc.wheel = ((uint16_t)temp[4])<<8 | ((uint16_t)temp[5]) ;
//	}
//}
