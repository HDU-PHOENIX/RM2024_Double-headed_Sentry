#include "vision.h"
#include "string.h"
#include "judge.h"
#include "INS_task.h"
#include "Gimbal.h"
#include "MyLib.h"
#include "user_define.h"
#include "usbd_cdc_if.h"
#include "MotorCtrl.h"

robot_id_t robot_id;

VisionValueType VisionValue;
VisionTransmitType VisionTransmit;

uint8_t vision_buf[VISION_BUFLEN];
uint8_t vision_transmit_buff[VISION_BUFLEN] = {"s000000000000000000000000000000e"};


uint32_t vision_missing_time=0;
int check_ok=0;

void vision_call_back_handler_Left_Control(uint8_t *buff)//对视觉接收数据进行处理
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if(buff[1] == 0xA0) // 左云台控制
        {
            check_ok++;  //接收正确计数
            vision_missing_time = 0;  //清零失联时间
					        
					
						VisionValue.find_bool = buff[2];  //是否识别到目标
						memcpy(&(VisionValue.vision_left_yaw_value.buff[0]),&buff[3],4);  //yaw 偏移量
						memcpy(&(VisionValue.vision_left_pitch_value.buff[0]),&buff[7],4);  // pitch 偏移量
						memcpy(&(VisionValue.vision_Left_Gimbal_distance.buff[0]),&buff[11],4);  // 装甲板距离
				
        }
    }
		if( (VisionValue.vision_left_yaw_value.value > 100 ) || (VisionValue.vision_left_pitch_value.value > 100 ) )
    {
        VisionValue.vision_left_yaw_value.value = 0 ;
        VisionValue.vision_left_pitch_value.value = 0 ;
    }        
}	

void vision_call_back_handler_Judge_msg(uint8_t *buff)//对视觉接收数据进行处理
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if(buff[1] == 0xA5) //裁判系统信息
        {
            check_ok++;  //接收正确计数
            vision_missing_time = 0;  //清零失联时间
					        
						memcpy(&(VisionValue.left_real_heat.buff[0]),&buff[2],4);
						memcpy(&(VisionValue.right_real_heat.buff[0]),&buff[6],4);
						memcpy(&(VisionValue.left_bullet_speed.buff[0]),&buff[10],4);
						memcpy(&(VisionValue.right_bullet_speed.buff[0]),&buff[14],4); 					
				
        }
    }
}	



float YAW_Vision,PITCH_Vision;
void vision_transmit_package_AutoaimFeedback (uint8_t *buff)
{
	
//0.63
	  YAW_Vision = ((Motor_6020[0].angle - 4096.f) / 4096.f ) * PI + 1.04694188f;
	  PITCH_Vision = Motor_4310.angle + 0.893f;
	
	  memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
	
		vision_transmit_buff[0] = 's';	//起始位
		vision_transmit_buff[1] = 0xB0; //自瞄信息
		memcpy(&(vision_transmit_buff[2]),&YAW_Vision, 4);	//yaw
		memcpy(&(vision_transmit_buff[6]),&PITCH_Vision, 4);	//pitch

		vision_transmit_buff[31] = 'e'; //结束位
}



void vision_transmit(){


	
		
	Gimbal_Data.Shoot_Data.vision_mode = 1;
	
	vision_transmit_package_AutoaimFeedback(vision_transmit_buff);
	CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
	

}

	#define	COMPENSATION_YAW	0
	#define	COMPENSATION_PITCH	0
	#define COMPENSATION_PITCH_DIST 0
	float SB_K_comps = 3.f;

//角度初始化补偿
float Vision_Comps_Yaw   = COMPENSATION_YAW;
float Vision_Comps_Pitch = COMPENSATION_PITCH;//固定补偿，减小距离的影响
float Vision_Comps_Pitch_Dist = COMPENSATION_PITCH_DIST;//根据距离补偿

/*******************************视觉误差获取*************************************/

//视觉是否发了新数据,FALSE没有,TRUE发了新的
uint8_t Vision_Get_New_Data = FALSE;

//打符是否换装甲了
uint8_t Vision_Armor = FALSE;

/**
  * @brief  判断视觉数据更新了吗
  * @param  void
  * @retval TRUE更新了   FALSE没更新
  * @attention  为自瞄做准备,串口空闲中断每触发一次且通过校验,则Vision_Get_New_Data置TRUE
  */
bool Vision_If_Update(void)
{
	return Vision_Get_New_Data;
}

/**
  * @brief  视觉数据更新标志位手动置0(false)
  * @param  void
  * @retval void
  * @attention  记得要清零,在哪清零自己选,调用这个函数就行
  */
void Vision_Clean_Update_Flag(void)
{
	Vision_Get_New_Data = FALSE;
}




/**
  * @brief  判断换装甲板了吗
  * @param  void
  * @retval TRUE换了   FALSE没换
  * @attention  为自动打符做准备,串口空闲中断每触发一次且通过校验,则Vision_Armor置TRUE
  */
bool Vision_If_Armor(void)
{
	return Vision_Armor;
}

/**
  * @brief  换装甲标志位手动置0(false)
  * @param  void
  * @retval void
  * @attention  记得要清零,在哪清零自己选,调用这个函数就行
  */
void Vision_Clean_Ammor_Flag(void)
{
	Vision_Armor = FALSE;
}

