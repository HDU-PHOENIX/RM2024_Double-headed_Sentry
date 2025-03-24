/**
  ******************************************************************************
  * @file    judge.c
  * @brief   解析裁判系统发来的数据，以获得机器人当前的状态数据。
  *
  ******************************************************************************
  * @attention
  *
  * 2021.3 已经完成了和tuxin.c的适配。
  *
  *
  *
  ******************************************************************************
  */

#include "judge.h"
#include "usart.h"
#include "crc.h"
#include "string.h"
#include "global_can.h"
frame_t judge_frame_rx = {0};
frame_t judge_frame_tx;
uint8_t judge_rx_buff[JUDGE_MAX_LENGTH];
robot_type_t robot_type;

bool Judge_Data_TF = false;//裁判数据是否可用，辅助函数调用
uint8_t Judge_Self_ID;//当前机器人的ID
uint16_t Judge_SelfClient_ID;//发送者机器人对应的客户端ID
/**************裁判系统数据辅助****************/
uint16_t ShootNum=0;//统计发弹量,0x0003触发一次则认为发射了一颗
uint16_t Shoot2Num=0;
bool Hurt_Data_Update = false;//装甲板伤害数据是否更新,每受一次伤害置TRUE,然后立即置FALSE,给底盘闪避用
uint16_t Hurt_num = 0;
#define BLUE  1
#define RED   0

union bulletdata
	{
		uint16_t bullet_speed_buff[2];
		float bullet_speed;
	}BulletSpeed;
	
	
	
uint16_t choose_client(uint8_t robot_id)
{
    uint16_t client_id;
    switch (robot_id)
    {
    case red_hero:
        client_id = red_hero_client;
        break;
    case red_engineer:
        client_id = red_engineer_client;
        break;
    case red_infantry_3:
        client_id = red_infantry_3_client;
        break;
    case red_infantry_4:
        client_id = red_infantry_4_client;
        break;
    case red_infantry_5:
        client_id = red_infantry_5_client;
        break;
    case red_aerial:
        client_id = red_aerial_client;
        break;

    case blue_hero:
        client_id = blue_hero_client;
        break;
    case blue_engineer:
        client_id = blue_engineer_client;
        break;
    case blue_infantry_3:
        client_id = blue_infantry_3_client;
        break;
    case blue_infantry_4:
        client_id = blue_infantry_4_client;
        break;
    case blue_infantry_5:
        client_id = blue_infantry_5_client;
        break;
    case blue_aerial:
        client_id = blue_aerial_client;
        break;
    default:
        break;
    }
    return client_id;
}

void Judge_Communication_Init(UART_HandleTypeDef* huart)
{
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能idle中断,用于裁判系统读取
    HAL_UART_Receive_DMA(huart, judge_rx_buff, 2000); //打开DMA接收，数据存入rx_buffer_judge数组中。
}


////从裁判系统读取数据
//void Judge_Read_Data(uint16_t Header ,uint8_t *data)
//{

//    //无数据包，则不作任何处理
//    if (data == NULL)
//    {
//        return;
//    }
//                switch(Header)
//                {

////                case game_status_t:
////                    judge_frame_rx.data.game_status.game_type=((uint16_t)data[0]<<8)+data[1];
////										judge_frame_rx.data.game_status.game_progress=((uint16_t)data[2]<<8)+data[3];
////										judge_frame_rx.data.game_status.stage_remain_time=((uint16_t)data[4]<<8)+data[5];
////                    break ;
//								
//                case CANID_game_robot_status:
//                    judge_frame_rx.data.game_robot_status.remain_HP=((uint16_t)data[2]<<8)+data[3];
////										judge_frame_rx.data.game_robot_status.max_HP=((uint16_t)data[4]<<8)+data[5];
//										judge_frame_rx.data.game_status.stage_remain_time = ((uint16_t)data[4]<<8)+data[5];
//                    Judge_Self_ID = judge_frame_rx.data.game_robot_status.robot_id =data[0];
//										judge_frame_rx.data.game_robot_status.shooter_id1_17mm_speed_limit=data[1];
//										judge_frame_rx.data.game_robot_status.shooter_id1_17mm_cooling_limit=((uint16_t)data[6]<<8)+data[7];
////										judge_frame_rx.data.game_status.stage_remain_time = ((uint16_t)data[6]<<8)+data[7];
//                    if(Judge_Self_ID < 100)
//                    {
//                        robot_type = red_robot;
//                    }
//                    else
//                    {
//                        robot_type = blue_robot;
//                    }
////										vision_send_pack();
//                    break;

//                case CANID_power_heat_data:
//                    judge_frame_rx.data.power_heat_data.shooter_id1_17mm_cooling_heat =((uint16_t)data[0]<<8)+data[1] ;
//										judge_frame_rx.data.power_heat_data.chassis_power_buffer = ((uint16_t)data[2]<<8)+data[3] ;
//                    break;

////                case robot_hurt_t:
//////                    memcpy(&judge_frame_rx.data.robot_hurt, data + DATA, len_robot_hurt);
//////								
//////                    if(judge_frame_rx.data.robot_hurt.hurt_type == 0)//装甲伤害扣血
//////                    {
//////                        Hurt_Data_Update = true;	//装甲数据每更新一次则判定为受到一次伤害
//////                        Hurt_num++;
//////                    }
//////                    else
//////                    {
//////                        Hurt_Data_Update = false;
//////                    }
////                    break;

//                case CANID_shoot_data:
//                    judge_frame_rx.data.shoot_data.bullet_type=data[0];
//										judge_frame_rx.data.shoot_data.shooter_id=data[1];
//										judge_frame_rx.data.shoot_data.bullet_freq=data[3];
//										BulletSpeed.bullet_speed_buff[0]=((uint16_t)data[4]<<8)+data[5];
//										BulletSpeed.bullet_speed_buff[1]=((uint16_t)data[6]<<8)+data[7];
//										judge_frame_rx.data.shoot_data.bullet_speed=BulletSpeed.bullet_speed;
////                    JUDGE_ShootNumCount();//发弹量统计
//                    break;

////                case bullet_remaining_t:
////                    memcpy(&judge_frame_rx.data.bullet_remaining, data + DATA, len_bullet_remaining);
////                    break;


/////*学生通信*/
////                case student_interactive_header_data_t:
////                    memcpy(&receive_student_data, data + STU_DATA, len_student_interactive_header_data);
////                    break;
//                default:
//                    break;
//                }
//        Judge_Data_TF = true;//辅助函数用
//}


#define send_max_len     200
unsigned char CliendTxBuffer[send_max_len];

#define Teammate_max_len     200
unsigned char TeammateTxBuffer[Teammate_max_len];
bool Send_Color = 0;
bool First_Time_Send_Commu = true;
uint16_t send_time = 0;

bool Color;
bool is_red_or_blue(void)//判断自己红蓝方
{
    Judge_Self_ID = judge_frame_rx.data.game_robot_status.robot_id;//读取当前机器人ID

    if(judge_frame_rx.data.game_robot_status.robot_id > 10)
    {
        return BLUE;
    }
    else
    {
        return RED;
    }
}

void determine_ID(void)
{
    Color = is_red_or_blue();
    if(Color == BLUE)
    {
        Judge_SelfClient_ID = 0x0164 + (Judge_Self_ID - 100); //计算客户端id
    }
    else if(Color == RED)
    {
        Judge_SelfClient_ID = 0x0100 + Judge_Self_ID;//计算客户端ID
    }
}

/********************裁判数据辅助判断函数***************************/

/**
  * @brief  数据是否可用
  * @param  void
  * @retval  TRUE可用   FALSE不可用
  * @attention  在裁判读取函数中实时改变返回值
  */
bool JUDGE_sGetDataState(void)
{
    return Judge_Data_TF;
}

/**
  * @brief  读取瞬时功率
  * @param  void
  * @retval 实时功率值
  * @attention
  */
float JUDGE_fGetChassisPower(void)
{
    return (judge_frame_rx.data.power_heat_data.chassis_power);
}

/**
  * @brief  读取底盘功率限制
  * @param  void
  * @retval 实时底盘功率限制
  * @attention
  */
uint8_t JUDGE_usGetPowerLimit(void)
{
    return (judge_frame_rx.data.game_robot_status.chassis_power_limit);
}
/**
  * @brief  读取剩余焦耳能量
  * @param  void
  * @retval 剩余缓冲焦耳能量
  * @attention
  */
uint16_t JUDGE_fGetRemainEnergy(void)
{
    return (judge_frame_rx.data.power_heat_data.chassis_power_buffer);
}

/**
  * @brief  读取当前等级
  * @param  void
  * @retval 当前等级
  * @attention
  */
uint8_t JUDGE_ucGetRobotLevel(void)
{
    return	judge_frame_rx.data.game_robot_status.robot_level;
}


/**
  * @brief  读取枪口热量
  * @param  void
  * @retval 
  * @attention  实时热量
  */
uint16_t JUDGE_usGetRemoteHeat17_1(void)
{
    return judge_frame_rx.data.power_heat_data.shooter_id1_17mm_cooling_heat;
}
uint16_t JUDGE_usGetRemoteHeat17_2(void)
{
    return judge_frame_rx.data.power_heat_data.shooter_id2_17mm_cooling_heat;
}
/**
  * @brief  读取比赛类型
  * @param  void
  * @retval void
  * @attention  联盟赛超级对抗赛
  */
uint8_t JUDGE_Game_Type(void)
{
    return judge_frame_rx.data.game_status.game_type;
}

/**
  * @brief  读取比赛阶段
  * @param  void
  * @retval void
  * @attention  实时比赛阶段
  */
uint8_t JUDGE_Game_Status(void)
{
    return judge_frame_rx.data.game_status.game_progress;
}

/**
  * @brief  读取射速
  * @param  void
  * @retval 17mm
  * @attention  实时射速
  */
float JUDGE_usGetSpeedHeat17(void)
{
    return judge_frame_rx.data.shoot_data.bullet_speed;
}

/**
  * @brief  统计发弹量
  * @param  void
  * @retval void
  * @attention
  */

float Shoot_Speed_Now = 0;
float Shoot_Speed_Last = 0;
void JUDGE_ShootNumCount(void)
{
	Shoot_Speed_Now = judge_frame_rx.data.shoot_data.bullet_speed;
	if(Shoot_Speed_Last != Shoot_Speed_Now)//因为是float型，几乎不可能完全相等,所以速度不等时说明发射了一颗弹
	{
			ShootNum++;
			Shoot_Speed_Last = Shoot_Speed_Now;
	}
}

/**
  * @brief  读取发弹量
  * @param  void
  * @retval 发弹量
  * @attention 不适用于双枪管
  */
uint16_t JUDGE_usGetShootNum(void)
{
    return ShootNum;
}
uint16_t JUDGE_usGetShootSum(void)
{
    return ShootNum+Shoot2Num;
}

/**
  * @brief  发弹量清零
  * @param  void
  * @retval void
  * @attention
  */
void JUDGE_ShootNum_Clear(void)
{
    ShootNum = 0;
}

/**
  * @brief  读取枪口热量
  * @param  void
  * @retval 当前等级热量上限
  * @attention
  */
uint16_t JUDGE_usGetHeatLimit(void)
{
    return judge_frame_rx.data.game_robot_status.shooter_id1_17mm_cooling_limit;
}
/**
  * @brief  读取枪口射速上限
  * @param  void
  * @retval 当前等级射速上限
  * @attention
  */
uint8_t JUDGE_usGetSpeedLimit(void)
{
    return judge_frame_rx.data.game_robot_status.shooter_id1_17mm_speed_limit;
}
/**
  * @brief  当前等级对应的枪口每秒冷却值
  * @param  void
  * @retval 当前等级冷却速度
  * @attention
  */
uint16_t JUDGE_usGetShootCold(void)
{
    return judge_frame_rx.data.game_robot_status.shooter_id1_17mm_cooling_rate;
}



bool Judge_If_Death(void)
{
    if(judge_frame_rx.data.game_robot_status.remain_HP == 0 && JUDGE_sGetDataState() == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}


float Last_HP = 600 ;
float Now_HP = 600;
uint8_t Hurt_Flag = 0;
bool  JUDGE_Is_Hurt(void)
{
	Now_HP = judge_frame_rx.data.game_robot_status.remain_HP;
	if(Now_HP < Last_HP)
    Hurt_Flag =TRUE;
	else
	{
	  Hurt_Flag = FALSE;
	}
		Last_HP = Now_HP;
	return Hurt_Flag;

}
float JUDGE_Read_HP(void)
{
	return judge_frame_rx.data.game_robot_status.remain_HP;
}
