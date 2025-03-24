#include "vision.h"
#include "string.h"
#include "judge.h"
#include "INS_task.h"
#include "Gimbal.h"
#include "MyLib.h"
#include "user_define.h"
#include "usbd_cdc_if.h"

robot_id_t robot_id;

VisionValueType VisionValue;
VisionTransmitType VisionTransmit;

uint8_t vision_buf[VISION_BUFLEN];
uint8_t vision_transmit_buff[VISION_BUFLEN] = {"s000000000000000000000000000000e"};


uint32_t vision_missing_time=0;
int check_ok=0;

union
	{
		float vx;
		int8_t data[4];
	}vision_speed_x;
	
union
	{
		float vy;
		int8_t data[4];
	}vision_speed_y;


void vision_call_back_handler_Chassis_Control(uint8_t *buff)//对视觉接收数据进行处理
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if (buff[1] == 0xA2) // 底盘控制
        {
            check_ok++;  //接收正确计数
            vision_missing_time = 0;  //清零失联时间
					        
            memcpy(&(VisionValue.vision_absolute_vx.buff[0]),&buff[2],4);  
						memcpy(&(VisionValue.vision_absolute_vy.buff[0]),&buff[6],4); 
        }
    }       
}

void vision_call_back_handler_Interaction_Control(uint8_t *buff)//对视觉接收数据进行处理
{
	
	    if( buff[0]=='s' && buff[31]=='e' )
      {
        if (buff[1] == 0xA3) //比赛交互控制
        {
            check_ok++;  //接收正确计数
            vision_missing_time = 0;  //清零失联时间
					        
            memcpy(&(VisionValue.decision.buff[0]),&buff[2],4);  //0：无  1：买活  2：买弹丸
						memcpy(&(VisionValue.game_content.buff[0]),&buff[6],4);  // 具体内容
        }
      }       
}

void vision_call_back_handler_Robotmode_Control(uint8_t *buff)//对视觉接收数据进行处理
{
	
	    if( buff[0]=='s' && buff[31]=='e' )
      {
        if (buff[1] == 0xA4) //比赛交互控制
        {
            check_ok++;  //接收正确计数
            vision_missing_time = 0;  //清零失联时间
					        
            memcpy(&(VisionValue.type.buff[0]),&buff[2],4);  //0：无  1：买活  2：买弹丸
						memcpy(&(VisionValue.robot_content.buff[0]),&buff[6],4);  // 具体内容
        }
      }       
}

//通过比赛剩余时间来判断大小符
void rune_judge(void)
{
	if(judge_frame_rx.data.game_status.stage_remain_time < 359 && judge_frame_rx.data.game_status.stage_remain_time > 240)
		VisionTransmit.rune_type = 1;
	else if(judge_frame_rx.data.game_status.stage_remain_time < 179 && judge_frame_rx.data.game_status.stage_remain_time > 0)
		VisionTransmit.rune_type = 2;
	else
		VisionTransmit.rune_type = 0;
}


void vision_transmit_handler(void)//对要发送给视觉的s数据进行处理
{
	
	if(judge_frame_rx.data.game_status.game_progress==4)//判断比赛是否开始
	{
	  VisionTransmit.game_time.value_offset=judge_frame_rx.data.game_status.stage_remain_time;//获取比赛进行时间
	}
	
  
	if(judge_frame_rx.data.game_robot_status.robot_id>100)//判断红蓝方
	{
		robot_id=blue;
    VisionTransmit.blue7_robot_HP.value_offset = judge_frame_rx.data.game_robot_status.remain_HP;//获取当前血量		
	}
	else
	{
		robot_id=red;
		VisionTransmit.red7_robot_HP.value_offset = judge_frame_rx.data.game_robot_status.remain_HP;//获取当前血量
	}
	
	//获取前哨站和基地血量
	if(robot_id==blue)
	{
		
	VisionTransmit.enemy_color.value_offset = 0;	//红
	VisionTransmit.blue_base_HP.value_offset=judge_frame_rx.data.game_robot_HP.blue_base_HP;
	VisionTransmit.blue_outpost_HP.value_offset=judge_frame_rx.data.game_robot_HP.blue_outpost_HP;
		
	}
	else if(robot_id==red)
	{
		
	VisionTransmit.enemy_color.value_offset = 1;	//蓝
	VisionTransmit.red_base_HP.value_offset=judge_frame_rx.data.game_robot_HP.red_base_HP;
	VisionTransmit.red_outpost_HP.value_offset=judge_frame_rx.data.game_robot_HP.red_outpost_HP;
		
	}
	

	//获取小地图指令坐标
	VisionTransmit.standard_7map_x.value_offset=judge_frame_rx.data.smallmap_communicate.target_position_x;
	VisionTransmit.standard_7map_y.value_offset=judge_frame_rx.data.smallmap_communicate.target_position_y;
	
	//读取弹速
	if(judge_frame_rx.data.shoot_data.shooter_id == 2)
	{
		VisionTransmit.right_bullet_speed.value_offset = judge_frame_rx.data.shoot_data.bullet_speed;
	}
	else if(judge_frame_rx.data.shoot_data.shooter_id == 1)
	{
		VisionTransmit.left_bullet_speed.value_offset = judge_frame_rx.data.shoot_data.bullet_speed;
	}
	//读取发弹量
	VisionTransmit.allowance_capacity.value_offset = JUDGE_Remaining();
	//读取经济
	VisionTransmit.game_economy.value_offset = judge_frame_rx.data.bullet_remaining.coin_remaining_num;
	
	//读取热量
	VisionTransmit.left_real_heat.value_offset = JUDGE_usGetRemoteHeat17_1();
	VisionTransmit.right_real_heat.value_offset = JUDGE_usGetRemoteHeat17_2();

	//读取血量
	VisionTransmit.blue1_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.blue_1_robot_HP;
	VisionTransmit.blue2_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.blue_2_robot_HP;
	VisionTransmit.blue3_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.blue_3_robot_HP;
	VisionTransmit.blue4_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.blue_4_robot_HP;
	VisionTransmit.blue5_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.blue_5_robot_HP;
	
	VisionTransmit.red1_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.red_1_robot_HP;
	VisionTransmit.red2_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.red_2_robot_HP;
	VisionTransmit.red3_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.red_3_robot_HP;
	VisionTransmit.red4_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.red_4_robot_HP;
	VisionTransmit.red5_robot_HP.value_offset = judge_frame_rx.data.game_robot_HP.red_5_robot_HP;
	
	//读取友方位置
	VisionTransmit.standard_1map_x.value_offset = judge_frame_rx.data.robot_position.hero_x;
	VisionTransmit.standard_1map_y.value_offset = judge_frame_rx.data.robot_position.hero_y;
	VisionTransmit.standard_2map_x.value_offset = judge_frame_rx.data.robot_position.engineer_x;
	VisionTransmit.standard_2map_y.value_offset = judge_frame_rx.data.robot_position.engineer_y;
	VisionTransmit.standard_3map_x.value_offset = judge_frame_rx.data.robot_position.standard_3_x;
	VisionTransmit.standard_3map_y.value_offset = judge_frame_rx.data.robot_position.standard_3_y;
	VisionTransmit.standard_4map_x.value_offset = judge_frame_rx.data.robot_position.standard_4_x;
	VisionTransmit.standard_4map_y.value_offset = judge_frame_rx.data.robot_position.standard_4_y;
	VisionTransmit.standard_5map_x.value_offset = judge_frame_rx.data.robot_position.standard_5_x;
	VisionTransmit.standard_5map_y.value_offset = judge_frame_rx.data.robot_position.standard_5_y;
	
  get_enemy_position();
	VisionTransmit.game_progress.value_offset = judge_frame_rx.data.game_status.game_progress;
	
}



void vision_transmit_package_Game(uint8_t *buff)
{

	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
	
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB3; //比赛信息

	 memcpy(&(vision_transmit_buff[2]),  &VisionTransmit.enemy_color.value_offset, 4);		//敌方颜色 0：红 1：蓝
	 memcpy(&(vision_transmit_buff[6]),  &VisionTransmit.game_progress.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.game_time.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.game_economy.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.allowance_capacity.value_offset, 4);
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.left_condition.value_offset, 4);
	 memcpy(&(vision_transmit_buff[26]),  &VisionTransmit.right_condition.value_offset, 4);
	
	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_Posistion123(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB1; //123机器人位置信息

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.standard_1map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.standard_1map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.standard_2map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.standard_2map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.standard_3map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.standard_3map_y.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_Posistion457(uint8_t *buff)
{
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB2; //457机器人位置信息

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.standard_4map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.standard_4map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.standard_5map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.standard_5map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.standard_7map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.standard_7map_y.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_redrobot_HP(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB4; //红方机器人血量信息

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.red1_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.red2_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.red3_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.red4_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.red5_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.red7_robot_HP.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //结束位
	

}

void vision_transmit_package_bluerobot_HP(uint8_t *buff)
{
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB5; //蓝方机器人血量信息

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.blue1_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.blue2_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.blue3_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.blue4_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.blue5_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.blue7_robot_HP.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_building_HP(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB6; //建筑血量信息

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.red_outpost_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.red_base_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.blue_outpost_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.blue_base_HP.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //结束位

}

void vision_transmit_package_Judge_msg(uint8_t *buff)
{
	
		memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB7; //发射状态量

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.left_real_heat.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.right_real_heat.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.left_bullet_speed.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.right_bullet_speed.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_radar_msg123(uint8_t *buff)
{
			
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB8; //敌方123位置（雷达）

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.enemy1_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.enemy1_y_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.enemy2_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.enemy2_y_position.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),   &VisionTransmit.enemy3_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),   &VisionTransmit.enemy3_y_position.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //结束位
}

void vision_transmit_package_radar_msg457(uint8_t *buff)
{
			
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//起始位
	 vision_transmit_buff[1] = 0xB9; //敌方457位置（雷达）

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.enemy4_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.enemy4_y_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.enemy5_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.enemy5_y_position.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),   &VisionTransmit.enemy7_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),   &VisionTransmit.enemy7_y_position.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //结束位
}



void vision_transmit(){
	
	int flag = 0;
	
	vision_transmit_handler();
	
	if(flag == 0)
	{
		vision_transmit_package_Posistion123(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}

  if(flag == 1)
	{
		vision_transmit_package_Posistion457(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	
	if(flag == 2)
	{
		vision_transmit_package_Game(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	
	if(flag == 3)
	{
		vision_transmit_package_redrobot_HP(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}

	if(flag == 4)
	{
		vision_transmit_package_bluerobot_HP(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	
	if(flag == 5)
	{
		vision_transmit_package_building_HP(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	if(flag == 6)
	{
		vision_transmit_package_Judge_msg(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	if(flag == 7)
	{
		vision_transmit_package_radar_msg123(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag ++;
	}
	if(flag == 8)
	{
		vision_transmit_package_radar_msg457(vision_transmit_buff);
		CDC_Transmit_FS(vision_transmit_buff, VISION_BUFLEN);
		flag = 0;
	}
	

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

