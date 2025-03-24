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

void vision_call_back_handler_Left_Control(uint8_t *buff)//���Ӿ��������ݽ��д���
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if(buff[1] == 0xA0) // ����̨����
        {
            check_ok++;  //������ȷ����
            vision_missing_time = 0;  //����ʧ��ʱ��
					        
					
						VisionValue.find_bool = buff[2];  //�Ƿ�ʶ��Ŀ��
						memcpy(&(VisionValue.vision_left_yaw_value.buff[0]),&buff[3],4);  //yaw ƫ����
						memcpy(&(VisionValue.vision_left_pitch_value.buff[0]),&buff[7],4);  // pitch ƫ����
						memcpy(&(VisionValue.vision_Left_Gimbal_distance.buff[0]),&buff[11],4);  // װ�װ����
				
        }
    }
		if( (VisionValue.vision_left_yaw_value.value > 100 ) || (VisionValue.vision_left_pitch_value.value > 100 ) )
    {
        VisionValue.vision_left_yaw_value.value = 0 ;
        VisionValue.vision_left_pitch_value.value = 0 ;
    }        
}	

void vision_call_back_handler_Judge_msg(uint8_t *buff)//���Ӿ��������ݽ��д���
{

    if( buff[0]=='s' && buff[31]=='e' )
    {
        if(buff[1] == 0xA5) //����ϵͳ��Ϣ
        {
            check_ok++;  //������ȷ����
            vision_missing_time = 0;  //����ʧ��ʱ��
					        
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
	
		vision_transmit_buff[0] = 's';	//��ʼλ
		vision_transmit_buff[1] = 0xB0; //������Ϣ
		memcpy(&(vision_transmit_buff[2]),&YAW_Vision, 4);	//yaw
		memcpy(&(vision_transmit_buff[6]),&PITCH_Vision, 4);	//pitch

		vision_transmit_buff[31] = 'e'; //����λ
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

