#ifndef _VISION_h
#define _VISION_h
#include "main.h"
#include "stdbool.h"
#include "judge.h"
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
	    0xA5     ����״̬��
      ��λ������λ��
      0xB0     ������Ϣ
      0xB1     �ҷ�������λ����Ϣ(1 2 3 �Ż�����)
      0xB2     �ҷ�������λ����Ϣ(4 5 7 �Ż�����)
      0xB3     ������Ϣ
      0xB4     �췽������Ѫ����Ϣ
      0xB5     ����������Ѫ����Ϣ
      0xB6     ����Ѫ����Ϣ
	    0xB7     ����״̬��
	    0xB8     �з�123λ��
			0xB9     �з�457λ��
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
	union
	{
		uint8_t buff[4];
		float value;
	}vision_YAW_value;
	
//���̿�������
	union
	{
		uint8_t buff[4];
		float value;
	}vision_absolute_vx;
	
	union
	{
		uint8_t buff[4];
		float value;
	}vision_absolute_vy;
	

//������������
	union
	{
		uint8_t buff[4];
		int value;
	}decision;//0���� 1����� 2����
	
	union
	{
		uint8_t buff[4];
		int value;
	}game_content;
	 
//����ģ���������
	union
	{
		uint8_t buff[4];
		int value;
	}type;//0���� 1��С���� 2������̨���� 3������̨����
	
	union
	{
		uint8_t buff[4];
		int value;
	}robot_content;
	 
}VisionValueType;

typedef struct{
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}enemy_color;
	uint8_t vision_mode;
	
	uint8_t rune_type;	//0�����ɼ��� 1��С�� 2�����	
	uint8_t fire_flag;  //�Ӿ����ƴ�  0������ 1����

	union
	{
		uint8_t buff[4];
		float value_offset;
	}vision_absolute_vx;
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}vision_absolute_vy;
	
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}game_progress;
	union
	{
		uint8_t buff[4];
		int value_offset;
	}game_time;
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}game_economy;// ��������
	
		union
	{
		uint8_t buff[4];
		int value_offset;
	}allowance_capacity;//��������
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}left_bullet_speed;// ����
	union
	{
		uint8_t buff[4];
		int value_offset;
	}right_bullet_speed;// �ҵ���
	
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}left_condition;
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}right_condition;
	

	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red1_robot_HP;//��Ӣ��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red2_robot_HP;//�칤��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red3_robot_HP;//�첽��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red4_robot_HP;//�첽��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red5_robot_HP;//�첽��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red7_robot_HP;//���ڱ�HP
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue1_robot_HP;//��Ӣ��HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue2_robot_HP;//������HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue3_robot_HP;//������HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue4_robot_HP;//������HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue5_robot_HP;//������HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue7_robot_HP;//���ڱ�HP
	
	
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_1map_x;// ���� 1 ��Ӣ�ۻ�����λ�� x ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_1map_y; // ���� 1 ��Ӣ�ۻ�����λ�� y ������
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_2map_x;// ���� 2 �Ź��̻�����λ�� x ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_2map_y;// ���� 2 �Ź��̻�����λ�� y ������
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_3map_x;// ���� 3 �Ų���������λ�� x ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_3map_y;	// ���� 3 �Ų���������λ�� y ������
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_4map_x;// ���� 4 �Ų���������λ�� y ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_4map_y;// ���� 4 �Ų���������λ�� y ������
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_5map_x;// ���� 5 �Ų���������λ�� y ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_5map_y;// ���� 5 �Ų���������λ�� y ������
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_7map_x;// ���� 7 ���ڱ�������λ�� x ������
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_7map_y;// ���� 7 ���ڱ�������λ�� y ������
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}red_outpost_HP; // �췽ǰ��վѪ��
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}red_base_HP; // �췽����Ѫ��
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}blue_outpost_HP;// ����ǰ��վѪ��
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}blue_base_HP;// ��������Ѫ��
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}left_real_heat;//��ǹ��ʵʱ����
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}right_real_heat;//��ǹ��ʵʱ����
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy1_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy2_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy3_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy4_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy5_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy7_x_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy1_y_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy2_y_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy3_y_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy4_y_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy5_y_position;
	
		union
	{
		uint8_t buff[4];
		float value_offset;
	}enemy7_y_position;
	
	
}VisionTransmitType;



extern VisionValueType VisionValue;
extern VisionTransmitType VisionTransmit;

extern int che;

extern uint8_t vision_buf[];

extern uint8_t Vision_Send_Buff[16];

extern uint8_t vision_transmit_buff[VISION_BUFLEN];


extern void vision_call_back_handler_Chassis_Control(uint8_t *buff);
extern void vision_call_back_handler_Interaction_Control(uint8_t *buff);
extern void vision_call_back_handler_Robotmode_Control(uint8_t *buff);


extern void vision_transmit(void);
extern void vision_transmit_package_Game(uint8_t *buff);
extern void vision_transmit_package_Posistion123(uint8_t *buff);
extern void vision_transmit_package_Posistion457(uint8_t *buff);
extern void vision_transmit_package_redrobot_HP(uint8_t *buff);
extern void vision_transmit_package_bluerobot_HP(uint8_t *buff);
extern void vision_transmit_package_building_HP(uint8_t *buff);
extern void vision_transmit_package_radar_msg123(uint8_t *buff);
extern void vision_transmit_package_radar_msg457(uint8_t *buff);


#endif


