#ifndef _VISION_h
#define _VISION_h
#include "main.h"
#include "stdbool.h"
#include "Shoot.h"
#define VISION_HUART huart8
#define VISION_MAX_LEN 50
#define VISION_BUFLEN 32
#define VISION_SEND 4

typedef struct Protocol_s     //通信协议
{
    uint8_t start;           //帧头，取's'   0 - 1
    uint8_t type;            //消息类型      1 - 2

    /* 
      上位机发下位机
      0xA0     小云台控制
	    0xA1     大云台控制
	    0xA2     底盘控制
	    0xA3     比赛交互控制
      0xA4     车体模块控制
      下位机发上位机
      0xB0     自瞄信息
      0xB1     我方机器人位置信息(1 2 3 号机器人)
      0xB2     我方机器人位置信息(4 5 7 号机器人)
      0xB3     比赛信息
      0xB4     红方机器人血量信息
      0xB5     蓝方机器人血量信息
      0xB6     建筑血量信息
    */

    uint8_t buffer[29];       // 数据          2 - 31
    uint8_t end;              // 帧尾，取 'e'  31 - 32
} Message;



typedef enum
{
	
 blue=1,
 red=0,

}robot_id_t;


extern uint8_t vision_buf[VISION_BUFLEN];
extern uint8_t vision_transmit_buff[VISION_BUFLEN];




typedef struct{
	

//云台控制内容
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
	
	uint8_t rune_type;	//0：不可激活 1：小符 2：大符	
	
	
	union
	{
		uint8_t buff[4];//联合体占用公用的内存
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
	}allowance_capacity;//允许发弹量
	
	
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


