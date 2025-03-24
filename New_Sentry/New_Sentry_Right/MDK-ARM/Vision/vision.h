#ifndef _VISION_h
#define _VISION_h
#include "main.h"
#include "stdbool.h"
#include "Shoot.h"
#define VISION_HUART huart8
#define VISION_MAX_LEN 50
#define VISION_BUFLEN 32
#define VISION_SEND 4

typedef struct Protocol_s     //ͨ��Э��
{
    uint8_t start;           //֡ͷ��ȡ's'   0 - 1
    uint8_t type;            //��Ϣ����      1 - 2

    /* 
      ��λ������λ��
      0xA0     С��̨����
	    0xA1     ����̨����
	    0xA2     ���̿���
	    0xA3     ������������
      0xA4     ����ģ�����
      ��λ������λ��
      0xB0     ������Ϣ
      0xB1     �ҷ�������λ����Ϣ(1 2 3 �Ż�����)
      0xB2     �ҷ�������λ����Ϣ(4 5 7 �Ż�����)
      0xB3     ������Ϣ
      0xB4     �췽������Ѫ����Ϣ
      0xB5     ����������Ѫ����Ϣ
      0xB6     ����Ѫ����Ϣ
    */

    uint8_t buffer[29];       // ����          2 - 31
    uint8_t end;              // ֡β��ȡ 'e'  31 - 32
} Message;



typedef enum
{
	
 blue=1,
 red=0,

}robot_id_t;


extern uint8_t vision_buf[VISION_BUFLEN];
extern uint8_t vision_transmit_buff[VISION_BUFLEN];




typedef struct{
	

//��̨��������
	uint8_t   find_bool;
		
	uint8_t   center_flag;
	
	union
	{
		uint8_t buff[4];
		int value;
	}real_heat;
	
	union
	{
		uint8_t buff[4];
		float value;
	}vision_right_yaw_value;
	union
	{
		uint8_t buff[4];
		float value;
	}vision_right_pitch_value;
	union
	{
		uint8_t buff[4];
		float value;
	}vision_Right_Gimbal_distance;
	
		union
	{
		uint8_t buff[4];
		float value;
	}left_bullet_speed;
	 
	union
	{
		uint8_t buff[4];
		float value;
	}right_bullet_speed;
	
		union
	{
		uint8_t buff[4];
		int value;
	}left_real_heat;
	
	union
	{
		uint8_t buff[4];
		int value;
	}right_real_heat;
	
	union
	{
		uint8_t buff[4];
		int value;
	}game_progress;
}VisionValueType;


typedef struct{
	
	uint8_t enemy_color;
	uint8_t vision_mode;
	
	uint8_t rune_type;	//0�����ɼ��� 1��С�� 2�����	
	
	
	union
	{
		uint8_t buff[4];//������ռ�ù��õ��ڴ�
		float value_offset;
	}vision_right_yaw_gyro;
	union
	{
		uint8_t buff[4];
		float value_offset;
	}vision_right_pitch_gyro;

	
		union
	{
		uint8_t buff[2];
		int value_offset;
	}allowance_capacity;//��������
	
	
}VisionTransmitType;



extern VisionValueType VisionValue;
extern VisionTransmitType VisionTransmit;

extern int che;
extern float PITCH_Vision,YAW_Vision;

extern uint8_t vision_buf[];

extern uint8_t Vision_Send_Buff[16];

extern uint8_t vision_transmit_buff[VISION_BUFLEN];

extern void vision_call_back_handler_Right_Control(uint8_t *buff);



extern void vision_transmit(void);

extern void vision_transmit_package_AutoaimFeedback(uint8_t *buff);
extern void	vision_call_back_handler_Judge_msg(uint8_t *buff);


#endif


