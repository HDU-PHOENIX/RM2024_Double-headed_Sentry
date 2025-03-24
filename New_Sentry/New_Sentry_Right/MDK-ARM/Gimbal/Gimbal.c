#include "Gimbal.h"
#include "Chassis.h"
#include "cmsis_os.h"
#include "MotorCtrl.h"
#include "MyLib.h"
#include "dbus.h"
#include "Gimbal.h"
#include "vision.h"
#include "user_define.h"

Gimbal_Type Gimbal_Data;


//#define PITCH_ANGLE_TOP 0.40f
//#define PITCH_ANGLE_BOTTOM -0.40f

#define PITCH_ANGLE_TOP 0.2f
#define PITCH_ANGLE_BOTTOM -0.2f

#define YAW_ANGLE_MAX 6800
#define YAW_ANGLE_MIN 1950



void StartGimbalTask(void const * argument)
{
	
	portTickType currentTime;
	Gimbal_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE ;
	while(1)
	{
		currentTime = xTaskGetTickCount(); //当前系统时间
		Robot_Mode_Select();
		Remote_Move();
		Vision_Move();
		vision_transmit();
		vTaskDelayUntil(&currentTime, 1); //绝对延时
	}
	
}


/**
  * @brief  机器人模式选择
  * @param  void
  * @retval 读取dbus数据判断机器人模式
  * @attention  无
  */
void Robot_Mode_Select(void)
{
	if( RC_Ctl.rc.s1 == 1 && RC_Ctl.rc.s2 == 2 )
	{
		Gimbal_Data.RobotData.robot_mode = AUTO_MODE;
		Gimbal_Data.Shoot_Data.use_vision = 1;
	}	
	else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 2 )
	{
		Gimbal_Data.RobotData.robot_mode = FOLLOW_MODE;   // 底盘跟随模式
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 3)
	{
		Gimbal_Data.RobotData.robot_mode = UNFOLLOW_MODE;  // 不跟随
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1)
	{
		Gimbal_Data.RobotData.robot_mode = SHOOT_MODE;	    //开启摩擦轮
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2){
		Gimbal_Data.RobotData.robot_mode = SPINNING;		   // 小陀螺
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 3){
		Gimbal_Data.RobotData.robot_mode = FIX_MODE;		   // 大YAW固定
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	else
	{
		Gimbal_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE;
		Gimbal_Data.Shoot_Data.use_vision = 0;
	}
	
	
}








/***************************************************
使用视觉模式时，让视觉来控制云台运动 
***************************************************/

struct error
	{
		uint16_t gimbal_pitch;
		uint16_t vision_yaw;
		uint16_t vision_pitch;
		uint16_t gimbal_yaw;
	}err_sum={0};	//记录数据错误次数


float yaw_value;
float pitch_value;
uint8_t allow_shoot = 1;
float yaw_fdb;	

float filter_yaw[2] = {0.1,0.9};
float filter_pitch[2] = {0.1,0.9};
float filter_pitch_zimiao[2]={0.1,0.9};
	
void Vision_Move(void)
{
	if(Gimbal_Data.Shoot_Data.use_vision == 1)	//再次判断是否使用视觉模式
	{
		//检测数据是否正确
		if(isnan(VisionValue.vision_right_pitch_value.value)||isinf(VisionValue.vision_right_pitch_value.value))
		{
			err_sum.vision_pitch++;
			return ;
		}
		if(isnan(VisionValue.vision_right_yaw_value.value)||isinf(VisionValue.vision_right_yaw_value.value))
		{
			err_sum.vision_yaw++;
			return;
		}
		if(VisionValue.find_bool == '0')
		{
			yaw_value = 0;
			pitch_value = 0;
			VisionValue.center_flag =  '0' ;
		}
		else
		{
			yaw_value = VisionValue.vision_right_yaw_value.value - YAW_Vision;
			pitch_value = VisionValue.vision_right_pitch_value.value - PITCH_Vision;
			//自瞄
			if((fabs(yaw_value) < ((0.135f /(2.f * VisionValue.vision_Right_Gimbal_distance.value)) * 0.7f)  && fabs(pitch_value) < ((0.135f /(2.f * VisionValue.vision_Right_Gimbal_distance.value)) * 0.7f)) && VisionValue.find_bool == '1') 
			{
				VisionValue.center_flag = '1';
			}
			else
			{
				VisionValue.center_flag = '0';
			}
			if(Gimbal_Data.Shoot_Data.vision_mode == 1 && VisionValue.find_bool == '1')
			{
				Gimbal_Data.IMUData.yaw_target = ((VisionValue.vision_right_yaw_value.value + PI - 0.99401953359375f) / PI) * 4096;
				Gimbal_Data.IMUData.pitch_target = -VisionValue.vision_right_pitch_value.value;
			}
			else
			{
				//打符
				if(VisionValue.find_bool == '1')
				{
					Gimbal_Data.IMUData.yaw_target = ((VisionValue.vision_right_yaw_value.value + PI - 0.99401953359375f) / PI) * 4096;
					Gimbal_Data.IMUData.pitch_target = -VisionValue.vision_right_pitch_value.value;
					Gimbal_Data.IMUData.yaw_target = Gimbal_Data.IMUData.yaw_target*filter_yaw[0] + Gimbal_Data.IMUData.ol[0]*filter_yaw[1];
					Gimbal_Data.IMUData.pitch_target = Gimbal_Data.IMUData.pitch_target*filter_pitch[0] + Gimbal_Data.IMUData.ol[2]*filter_pitch[1];
				}
			}
	  }
	 }
}

int Scan_Flag_YAW_turn= 0;//顺时针转0，逆时针转1
int Scan_Flag_PITCH = 0;
int Scan_TIME;
double Scan_w =  _2PI / 500;
int Change_Mode_Flag;
int Change_Scan_Rage = 0;
float Pitch_Move_Speed = 0.001f;
float Yaw_Move_Speed = 1.5f;


void Remote_Move()
{
	if((RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1) || (RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2)|| (RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 3))
	{
			if(Gimbal_Data.Shoot_Data.use_vision == 0)
		{
			Gimbal_Data.IMUData.pitch_target += RC_Ctl.rc.ch3/660.f*0.001f;
			//Gimbal_Data.IMUData.yaw_target -= RC_Ctl.rc.ch2/660.f;
		}
	}
	else if(Gimbal_Data.Shoot_Data.use_vision == 1)
	{
		if(VisionValue.find_bool == '1')
		{
			Change_Mode_Flag = 0;
		}
		if(VisionValue.find_bool == '0' )
		{
			Change_Mode_Flag ++ ;
			if(Change_Mode_Flag > 800)
			{
				//pitch
			Change_Mode_Flag = 800;
			if(Scan_Flag_PITCH == 1)
			{
				Gimbal_Data.IMUData.pitch_target -= Pitch_Move_Speed;
				
				if(Gimbal_Data.IMUData.pitch_target < PITCH_ANGLE_BOTTOM)
				{
					Gimbal_Data.IMUData.pitch_target = PITCH_ANGLE_BOTTOM;
					Scan_Flag_PITCH = 0;
				}
			}
			else if((Gimbal_Data.IMUData.pitch_target < PITCH_ANGLE_TOP) || (Scan_Flag_PITCH == 0))
			{
				Gimbal_Data.IMUData.pitch_target += Pitch_Move_Speed;
				if(Gimbal_Data.IMUData.pitch_target > PITCH_ANGLE_TOP)
				{
					Gimbal_Data.IMUData.pitch_target = PITCH_ANGLE_TOP;
					Scan_Flag_PITCH = 1;
				}
			}
//			//pitch
//			Gimbal_Data.IMUData.pitch_target = sin(Scan_w * Scan_TIME);
//			Scan_TIME = (Scan_TIME + 1) % 1000;
			
			
			//yaw
			if(Motor_6020[0].angle > 6700 && Motor_6020[0].angle < YAW_ANGLE_MAX)
			{
				Gimbal_Data.IMUData.yaw_target -= Yaw_Move_Speed;
				Scan_Flag_YAW_turn = 1;
			}
			else if(Motor_6020[0].angle > YAW_ANGLE_MIN && Motor_6020[0].angle < 2050)
			{
				Gimbal_Data.IMUData.yaw_target += Yaw_Move_Speed;
				Scan_Flag_YAW_turn = 0;
			}
			else
			{
				if(Scan_Flag_YAW_turn == 0)
				{
					Gimbal_Data.IMUData.yaw_target += Yaw_Move_Speed;
				}
				else
				{
					Gimbal_Data.IMUData.yaw_target -= Yaw_Move_Speed;
				}
			}
		 }
		}
		else
		{
			Change_Mode_Flag = 0;
		}
	}
}

