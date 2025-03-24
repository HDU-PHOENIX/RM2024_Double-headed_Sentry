/**
  ******************************************************************************
  * @file    judge.c
  * @brief   ��������ϵͳ���������ݣ��Ի�û����˵�ǰ��״̬���ݡ�
  *
  ******************************************************************************
  * @attention
  *
  * 2021.3 �Ѿ�����˺�tuxin.c�����䡣
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

bool Judge_Data_TF = false;//���������Ƿ���ã�������������
uint8_t Judge_Self_ID;//��ǰ�����˵�ID
uint16_t Judge_SelfClient_ID;//�����߻����˶�Ӧ�Ŀͻ���ID
/**************����ϵͳ���ݸ���****************/
uint16_t ShootNum=0;//ͳ�Ʒ�����,0x0003����һ������Ϊ������һ��
uint16_t Shoot2Num=0;
bool Hurt_Data_Update = false;//װ�װ��˺������Ƿ����,ÿ��һ���˺���TRUE,Ȼ��������FALSE,������������
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
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//ʹ��idle�ж�,���ڲ���ϵͳ��ȡ
    HAL_UART_Receive_DMA(huart, judge_rx_buff, 2000); //��DMA���գ����ݴ���rx_buffer_judge�����С�
}


////�Ӳ���ϵͳ��ȡ����
//void Judge_Read_Data(uint16_t Header ,uint8_t *data)
//{

//    //�����ݰ��������κδ���
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
//////                    if(judge_frame_rx.data.robot_hurt.hurt_type == 0)//װ���˺���Ѫ
//////                    {
//////                        Hurt_Data_Update = true;	//װ������ÿ����һ�����ж�Ϊ�ܵ�һ���˺�
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
////                    JUDGE_ShootNumCount();//������ͳ��
//                    break;

////                case bullet_remaining_t:
////                    memcpy(&judge_frame_rx.data.bullet_remaining, data + DATA, len_bullet_remaining);
////                    break;


/////*ѧ��ͨ��*/
////                case student_interactive_header_data_t:
////                    memcpy(&receive_student_data, data + STU_DATA, len_student_interactive_header_data);
////                    break;
//                default:
//                    break;
//                }
//        Judge_Data_TF = true;//����������
//}


#define send_max_len     200
unsigned char CliendTxBuffer[send_max_len];

#define Teammate_max_len     200
unsigned char TeammateTxBuffer[Teammate_max_len];
bool Send_Color = 0;
bool First_Time_Send_Commu = true;
uint16_t send_time = 0;

bool Color;
bool is_red_or_blue(void)//�ж��Լ�������
{
    Judge_Self_ID = judge_frame_rx.data.game_robot_status.robot_id;//��ȡ��ǰ������ID

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
        Judge_SelfClient_ID = 0x0164 + (Judge_Self_ID - 100); //����ͻ���id
    }
    else if(Color == RED)
    {
        Judge_SelfClient_ID = 0x0100 + Judge_Self_ID;//����ͻ���ID
    }
}

/********************�������ݸ����жϺ���***************************/

/**
  * @brief  �����Ƿ����
  * @param  void
  * @retval  TRUE����   FALSE������
  * @attention  �ڲ��ж�ȡ������ʵʱ�ı䷵��ֵ
  */
bool JUDGE_sGetDataState(void)
{
    return Judge_Data_TF;
}

/**
  * @brief  ��ȡ˲ʱ����
  * @param  void
  * @retval ʵʱ����ֵ
  * @attention
  */
float JUDGE_fGetChassisPower(void)
{
    return (judge_frame_rx.data.power_heat_data.chassis_power);
}

/**
  * @brief  ��ȡ���̹�������
  * @param  void
  * @retval ʵʱ���̹�������
  * @attention
  */
uint8_t JUDGE_usGetPowerLimit(void)
{
    return (judge_frame_rx.data.game_robot_status.chassis_power_limit);
}
/**
  * @brief  ��ȡʣ�ཹ������
  * @param  void
  * @retval ʣ�໺�役������
  * @attention
  */
uint16_t JUDGE_fGetRemainEnergy(void)
{
    return (judge_frame_rx.data.power_heat_data.chassis_power_buffer);
}

/**
  * @brief  ��ȡ��ǰ�ȼ�
  * @param  void
  * @retval ��ǰ�ȼ�
  * @attention
  */
uint8_t JUDGE_ucGetRobotLevel(void)
{
    return	judge_frame_rx.data.game_robot_status.robot_level;
}


/**
  * @brief  ��ȡǹ������
  * @param  void
  * @retval 
  * @attention  ʵʱ����
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
  * @brief  ��ȡ��������
  * @param  void
  * @retval void
  * @attention  �����������Կ���
  */
uint8_t JUDGE_Game_Type(void)
{
    return judge_frame_rx.data.game_status.game_type;
}

/**
  * @brief  ��ȡ�����׶�
  * @param  void
  * @retval void
  * @attention  ʵʱ�����׶�
  */
uint8_t JUDGE_Game_Status(void)
{
    return judge_frame_rx.data.game_status.game_progress;
}

/**
  * @brief  ��ȡ����
  * @param  void
  * @retval 17mm
  * @attention  ʵʱ����
  */
float JUDGE_usGetSpeedHeat17(void)
{
    return judge_frame_rx.data.shoot_data.bullet_speed;
}

/**
  * @brief  ͳ�Ʒ�����
  * @param  void
  * @retval void
  * @attention
  */

float Shoot_Speed_Now = 0;
float Shoot_Speed_Last = 0;
void JUDGE_ShootNumCount(void)
{
	Shoot_Speed_Now = judge_frame_rx.data.shoot_data.bullet_speed;
	if(Shoot_Speed_Last != Shoot_Speed_Now)//��Ϊ��float�ͣ�������������ȫ���,�����ٶȲ���ʱ˵��������һ�ŵ�
	{
			ShootNum++;
			Shoot_Speed_Last = Shoot_Speed_Now;
	}
}

/**
  * @brief  ��ȡ������
  * @param  void
  * @retval ������
  * @attention ��������˫ǹ��
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
  * @brief  ����������
  * @param  void
  * @retval void
  * @attention
  */
void JUDGE_ShootNum_Clear(void)
{
    ShootNum = 0;
}

/**
  * @brief  ��ȡǹ������
  * @param  void
  * @retval ��ǰ�ȼ���������
  * @attention
  */
uint16_t JUDGE_usGetHeatLimit(void)
{
    return judge_frame_rx.data.game_robot_status.shooter_id1_17mm_cooling_limit;
}
/**
  * @brief  ��ȡǹ����������
  * @param  void
  * @retval ��ǰ�ȼ���������
  * @attention
  */
uint8_t JUDGE_usGetSpeedLimit(void)
{
    return judge_frame_rx.data.game_robot_status.shooter_id1_17mm_speed_limit;
}
/**
  * @brief  ��ǰ�ȼ���Ӧ��ǹ��ÿ����ȴֵ
  * @param  void
  * @retval ��ǰ�ȼ���ȴ�ٶ�
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
