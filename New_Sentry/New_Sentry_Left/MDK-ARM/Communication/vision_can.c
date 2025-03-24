//#include "vision_can.h"
//#include "Gimbal.h"
//#include "MotorCtrl.h"
//#include "MyLib.h"
//#include "user_define.h"
//#include "Chassis.h"
//#include "global_can.h"
//#include "vision.h"
//#include "arm_math.h"
//#include "usart.h"


//uint8_t VISION_data_sand[20];

//void sand_imu_data(void) //UART
//{
//	
//		union
//	{
//		float YAW_Data;
//		int8_t data[4];
//	}Left_YAW;
//	
//	union
//	{
//		float PITCH_Data;
//		int8_t data[4];
//	}Left_PITCH;

//	
//	Left_PITCH.PITCH_Data = Gimbal_Data.IMUData.ol[2];
//	Left_YAW.YAW_Data = Gimbal_Data.IMUData.ol[0];
//	
//	VISION_data_sand[0] = 0xEE;
//	VISION_data_sand[1] = Left_PITCH.data[0];
//	VISION_data_sand[2] = Left_PITCH.data[1];
//	VISION_data_sand[3] = Left_PITCH.data[2];
//	VISION_data_sand[4] = Left_PITCH.data[3];
//	VISION_data_sand[5] = Left_YAW.data[0];
//	VISION_data_sand[6] = Left_YAW.data[1];
//	VISION_data_sand[7] = Left_YAW.data[2];
//	VISION_data_sand[8] = Left_YAW.data[3];
//	VISION_data_sand[9] = 0;
//	VISION_data_sand[10] = 0;
//	VISION_data_sand[11] = 0;
//	VISION_data_sand[12] = 0;
//	VISION_data_sand[13] = 0;
//	VISION_data_sand[14] = 0;
//	VISION_data_sand[15] = 0;
//	VISION_data_sand[16] = 0;
//	VISION_data_sand[17] = 0;
//	VISION_data_sand[18] = 0;

//	VISION_data_sand[19] = 0xEF;
//	

//	
//}


//void VISION_AIM_MSG_Update(uint8_t *data)
//{
//		// ´Óintµ½float×ª»»
//	for( int i = 0; i < 40 ; i++ )
//	{
//			if( data[i] == 0xFE && data[i + 19] == 0xFF )
//		{
//		
//			VisionValue.vision_left_yaw_value.buff[0] = data[i+1];
//			VisionValue.vision_left_yaw_value.buff[1] = data[i+2];
//			VisionValue.vision_left_yaw_value.buff[2] = data[i+3];
//			VisionValue.vision_left_yaw_value.buff[3] = data[i+4];

//			VisionValue.vision_left_pitch_value.buff[0] = data[i+5];
//			VisionValue.vision_left_pitch_value.buff[1] = data[i+6];
//			VisionValue.vision_left_pitch_value.buff[2] = data[i+7];
//			VisionValue.vision_left_pitch_value.buff[3] = data[i+8];
//			
//				
//			VisionValue.Left_center_flag      = data[i+9];
//			VisionValue.Left_center_flag_last = data[i+10];
//			VisionValue.Left_identify_target  = data[i+11];
//			i = i + 19;
//		}
//	}
//}

