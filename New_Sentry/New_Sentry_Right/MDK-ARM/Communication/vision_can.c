//#include "vision_can.h"
//#include "Gimbal.h"
//#include "MotorCtrl.h"
//#include "MyLib.h"
//#include "user_define.h"
//#include "Chassis.h"
//#include "vision.h"
//#include "global_can.h"
//#include "arm_math.h"
//#include "usart.h"

//Left_IMU_DATA Left_IMU;


//uint8_t VISION_data_sand[20];


//void sand_vision_aim_data(void)
//{	

//	VISION_data_sand[0] = 0xFE;

//	VISION_data_sand[1] = VisionValue.vision_left_yaw_value.buff[0];
//	VISION_data_sand[2] = VisionValue.vision_left_yaw_value.buff[1];
//	VISION_data_sand[3] = VisionValue.vision_left_yaw_value.buff[2];
//	VISION_data_sand[4] = VisionValue.vision_left_yaw_value.buff[3];
//	VISION_data_sand[5] = VisionValue.vision_left_pitch_value.buff[0];
//	VISION_data_sand[6] = VisionValue.vision_left_pitch_value.buff[1];
//	VISION_data_sand[7] = VisionValue.vision_left_pitch_value.buff[2];
//	VISION_data_sand[8] = VisionValue.vision_left_pitch_value.buff[3];
//	VISION_data_sand[9] = VisionValue.Left_center_flag;
//	VISION_data_sand[10] = VisionValue.Left_center_flag_last;
//	VISION_data_sand[11] = VisionValue.Left_identify_target;
//	VISION_data_sand[12] = 0;
//	VISION_data_sand[13] = 0;
//	VISION_data_sand[14] = 0;
//	VISION_data_sand[15] = 0;
//	VISION_data_sand[16] = 0;
//	VISION_data_sand[17] = 0;
//	VISION_data_sand[18] = 0;
//	
//	VISION_data_sand[19] = 0xFF;

//}


//void VISION_Ctrl_MSG_Update(uint8_t *data)
//{
//			
//	for( int i = 0; i < 40; i++ )
//	{
//		if( data[i] == 0xEE && data[ i + 19] == 0xEF )
//		{
//			Left_IMU.Left_PITCH.data[0] = data[i+1];
//			Left_IMU.Left_PITCH.data[1] = data[i+2];
//			Left_IMU.Left_PITCH.data[2] = data[i+3];
//			Left_IMU.Left_PITCH.data[3] = data[i+4];

//			Left_IMU.Left_YAW.data[0] = data[i+5];
//			Left_IMU.Left_YAW.data[1] = data[i+6];
//			Left_IMU.Left_YAW.data[2] = data[i+7];
//			Left_IMU.Left_YAW.data[3] = data[i+8];
//			i = i + 19;
//		}
//}

//}







