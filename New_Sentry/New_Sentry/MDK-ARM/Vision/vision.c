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


void vision_call_back_handler_Chassis_Control(uint8_t *buff)//���Ӿ��������ݽ��д���
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if (buff[1] == 0xA2) // ���̿���
        {
            check_ok++;  //������ȷ����
            vision_missing_time = 0;  //����ʧ��ʱ��
					        
            memcpy(&(VisionValue.vision_absolute_vx.buff[0]),&buff[2],4);  
						memcpy(&(VisionValue.vision_absolute_vy.buff[0]),&buff[6],4); 
        }
    }       
}

void vision_call_back_handler_Interaction_Control(uint8_t *buff)//���Ӿ��������ݽ��д���
{
	
	    if( buff[0]=='s' && buff[31]=='e' )
      {
        if (buff[1] == 0xA3) //������������
        {
            check_ok++;  //������ȷ����
            vision_missing_time = 0;  //����ʧ��ʱ��
					        
            memcpy(&(VisionValue.decision.buff[0]),&buff[2],4);  //0����  1�����  2������
						memcpy(&(VisionValue.game_content.buff[0]),&buff[6],4);  // ��������
        }
      }       
}

void vision_call_back_handler_Robotmode_Control(uint8_t *buff)//���Ӿ��������ݽ��д���
{
	
	    if( buff[0]=='s' && buff[31]=='e' )
      {
        if (buff[1] == 0xA4) //������������
        {
            check_ok++;  //������ȷ����
            vision_missing_time = 0;  //����ʧ��ʱ��
					        
            memcpy(&(VisionValue.type.buff[0]),&buff[2],4);  //0����  1�����  2������
						memcpy(&(VisionValue.robot_content.buff[0]),&buff[6],4);  // ��������
        }
      }       
}

//ͨ������ʣ��ʱ�����жϴ�С��
void rune_judge(void)
{
	if(judge_frame_rx.data.game_status.stage_remain_time < 359 && judge_frame_rx.data.game_status.stage_remain_time > 240)
		VisionTransmit.rune_type = 1;
	else if(judge_frame_rx.data.game_status.stage_remain_time < 179 && judge_frame_rx.data.game_status.stage_remain_time > 0)
		VisionTransmit.rune_type = 2;
	else
		VisionTransmit.rune_type = 0;
}


void vision_transmit_handler(void)//��Ҫ���͸��Ӿ���s���ݽ��д���
{
	
	if(judge_frame_rx.data.game_status.game_progress==4)//�жϱ����Ƿ�ʼ
	{
	  VisionTransmit.game_time.value_offset=judge_frame_rx.data.game_status.stage_remain_time;//��ȡ��������ʱ��
	}
	
  
	if(judge_frame_rx.data.game_robot_status.robot_id>100)//�жϺ�����
	{
		robot_id=blue;
    VisionTransmit.blue7_robot_HP.value_offset = judge_frame_rx.data.game_robot_status.remain_HP;//��ȡ��ǰѪ��		
	}
	else
	{
		robot_id=red;
		VisionTransmit.red7_robot_HP.value_offset = judge_frame_rx.data.game_robot_status.remain_HP;//��ȡ��ǰѪ��
	}
	
	//��ȡǰ��վ�ͻ���Ѫ��
	if(robot_id==blue)
	{
		
	VisionTransmit.enemy_color.value_offset = 0;	//��
	VisionTransmit.blue_base_HP.value_offset=judge_frame_rx.data.game_robot_HP.blue_base_HP;
	VisionTransmit.blue_outpost_HP.value_offset=judge_frame_rx.data.game_robot_HP.blue_outpost_HP;
		
	}
	else if(robot_id==red)
	{
		
	VisionTransmit.enemy_color.value_offset = 1;	//��
	VisionTransmit.red_base_HP.value_offset=judge_frame_rx.data.game_robot_HP.red_base_HP;
	VisionTransmit.red_outpost_HP.value_offset=judge_frame_rx.data.game_robot_HP.red_outpost_HP;
		
	}
	

	//��ȡС��ͼָ������
	VisionTransmit.standard_7map_x.value_offset=judge_frame_rx.data.smallmap_communicate.target_position_x;
	VisionTransmit.standard_7map_y.value_offset=judge_frame_rx.data.smallmap_communicate.target_position_y;
	
	//��ȡ����
	if(judge_frame_rx.data.shoot_data.shooter_id == 2)
	{
		VisionTransmit.right_bullet_speed.value_offset = judge_frame_rx.data.shoot_data.bullet_speed;
	}
	else if(judge_frame_rx.data.shoot_data.shooter_id == 1)
	{
		VisionTransmit.left_bullet_speed.value_offset = judge_frame_rx.data.shoot_data.bullet_speed;
	}
	//��ȡ������
	VisionTransmit.allowance_capacity.value_offset = JUDGE_Remaining();
	//��ȡ����
	VisionTransmit.game_economy.value_offset = judge_frame_rx.data.bullet_remaining.coin_remaining_num;
	
	//��ȡ����
	VisionTransmit.left_real_heat.value_offset = JUDGE_usGetRemoteHeat17_1();
	VisionTransmit.right_real_heat.value_offset = JUDGE_usGetRemoteHeat17_2();

	//��ȡѪ��
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
	
	//��ȡ�ѷ�λ��
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
	
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB3; //������Ϣ

	 memcpy(&(vision_transmit_buff[2]),  &VisionTransmit.enemy_color.value_offset, 4);		//�з���ɫ 0���� 1����
	 memcpy(&(vision_transmit_buff[6]),  &VisionTransmit.game_progress.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.game_time.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.game_economy.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.allowance_capacity.value_offset, 4);
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.left_condition.value_offset, 4);
	 memcpy(&(vision_transmit_buff[26]),  &VisionTransmit.right_condition.value_offset, 4);
	
	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_Posistion123(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB1; //123������λ����Ϣ

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.standard_1map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.standard_1map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.standard_2map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.standard_2map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.standard_3map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.standard_3map_y.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_Posistion457(uint8_t *buff)
{
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB2; //457������λ����Ϣ

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.standard_4map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.standard_4map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.standard_5map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.standard_5map_y.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.standard_7map_x.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.standard_7map_y.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_redrobot_HP(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB4; //�췽������Ѫ����Ϣ

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.red1_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.red2_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.red3_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.red4_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.red5_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.red7_robot_HP.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //����λ
	

}

void vision_transmit_package_bluerobot_HP(uint8_t *buff)
{
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB5; //����������Ѫ����Ϣ

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.blue1_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.blue2_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.blue3_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.blue4_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[18]),  &VisionTransmit.blue5_robot_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),  &VisionTransmit.blue7_robot_HP.value_offset, 4);	

	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_building_HP(uint8_t *buff)
{
	
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB6; //����Ѫ����Ϣ

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.red_outpost_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.red_base_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),   &VisionTransmit.blue_outpost_HP.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),   &VisionTransmit.blue_base_HP.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //����λ

}

void vision_transmit_package_Judge_msg(uint8_t *buff)
{
	
		memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB7; //����״̬��

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.left_real_heat.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.right_real_heat.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.left_bullet_speed.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.right_bullet_speed.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_radar_msg123(uint8_t *buff)
{
			
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB8; //�з�123λ�ã��״

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.enemy1_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.enemy1_y_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.enemy2_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.enemy2_y_position.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),   &VisionTransmit.enemy3_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),   &VisionTransmit.enemy3_y_position.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //����λ
}

void vision_transmit_package_radar_msg457(uint8_t *buff)
{
			
	 memset(vision_transmit_buff,0,sizeof(vision_transmit_buff));
		
	 vision_transmit_buff[0] = 's';	//��ʼλ
	 vision_transmit_buff[1] = 0xB9; //�з�457λ�ã��״

	 memcpy(&(vision_transmit_buff[2]),   &VisionTransmit.enemy4_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[6]),   &VisionTransmit.enemy4_y_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[10]),  &VisionTransmit.enemy5_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[14]),  &VisionTransmit.enemy5_y_position.value_offset, 4);
	 memcpy(&(vision_transmit_buff[18]),   &VisionTransmit.enemy7_x_position.value_offset, 4);	
	 memcpy(&(vision_transmit_buff[22]),   &VisionTransmit.enemy7_y_position.value_offset, 4);	
	
	 vision_transmit_buff[31] = 'e'; //����λ
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

//�Ƕȳ�ʼ������
float Vision_Comps_Yaw   = COMPENSATION_YAW;
float Vision_Comps_Pitch = COMPENSATION_PITCH;//�̶���������С�����Ӱ��
float Vision_Comps_Pitch_Dist = COMPENSATION_PITCH_DIST;//���ݾ��벹��

/*******************************�Ӿ�����ȡ*************************************/

//�Ӿ��Ƿ���������,FALSEû��,TRUE�����µ�
uint8_t Vision_Get_New_Data = FALSE;

//����Ƿ�װ����
uint8_t Vision_Armor = FALSE;

/**
  * @brief  �ж��Ӿ����ݸ�������
  * @param  void
  * @retval TRUE������   FALSEû����
  * @attention  Ϊ������׼��,���ڿ����ж�ÿ����һ����ͨ��У��,��Vision_Get_New_Data��TRUE
  */
bool Vision_If_Update(void)
{
	return Vision_Get_New_Data;
}

/**
  * @brief  �Ӿ����ݸ��±�־λ�ֶ���0(false)
  * @param  void
  * @retval void
  * @attention  �ǵ�Ҫ����,���������Լ�ѡ,���������������
  */
void Vision_Clean_Update_Flag(void)
{
	Vision_Get_New_Data = FALSE;
}




/**
  * @brief  �жϻ�װ�װ�����
  * @param  void
  * @retval TRUE����   FALSEû��
  * @attention  Ϊ�Զ������׼��,���ڿ����ж�ÿ����һ����ͨ��У��,��Vision_Armor��TRUE
  */
bool Vision_If_Armor(void)
{
	return Vision_Armor;
}

/**
  * @brief  ��װ�ױ�־λ�ֶ���0(false)
  * @param  void
  * @retval void
  * @attention  �ǵ�Ҫ����,���������Լ�ѡ,���������������
  */
void Vision_Clean_Ammor_Flag(void)
{
	Vision_Armor = FALSE;
}

