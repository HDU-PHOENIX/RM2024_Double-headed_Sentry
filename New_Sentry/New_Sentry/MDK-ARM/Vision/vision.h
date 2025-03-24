#ifndef _VISION_h
#define _VISION_h
#include "main.h"
#include "stdbool.h"
#include "judge.h"
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
	    0xA5     发射状态量
      下位机发上位机
      0xB0     自瞄信息
      0xB1     我方机器人位置信息(1 2 3 号机器人)
      0xB2     我方机器人位置信息(4 5 7 号机器人)
      0xB3     比赛信息
      0xB4     红方机器人血量信息
      0xB5     蓝方机器人血量信息
      0xB6     建筑血量信息
	    0xB7     发射状态量
	    0xB8     敌方123位置
			0xB9     敌方457位置
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
	union
	{
		uint8_t buff[4];
		float value;
	}vision_YAW_value;
	
//底盘控制内容
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
	

//比赛交互内容
	union
	{
		uint8_t buff[4];
		int value;
	}decision;//0：无 1：买活 2：买弹
	
	union
	{
		uint8_t buff[4];
		int value;
	}game_content;
	 
//车体模块控制内容
	union
	{
		uint8_t buff[4];
		int value;
	}type;//0：无 1：小陀螺 2：左云台单发 3：右云台单发
	
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
	
	uint8_t rune_type;	//0：不可激活 1：小符 2：大符	
	uint8_t fire_flag;  //视觉控制打弹  0不发射 1发射

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
	}game_economy;// 比赛经济
	
		union
	{
		uint8_t buff[4];
		int value_offset;
	}allowance_capacity;//允许发弹量
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}left_bullet_speed;// 左弹速
	union
	{
		uint8_t buff[4];
		int value_offset;
	}right_bullet_speed;// 右弹速
	
	
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
	}red1_robot_HP;//红英雄HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red2_robot_HP;//红工程HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red3_robot_HP;//红步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red4_robot_HP;//红步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red5_robot_HP;//红步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}red7_robot_HP;//红哨兵HP
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue1_robot_HP;//蓝英雄HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue2_robot_HP;//蓝工程HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue3_robot_HP;//蓝步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue4_robot_HP;//蓝步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue5_robot_HP;//蓝步兵HP
	union
	{
		uint8_t buff[4];
		int value_offset;
	}blue7_robot_HP;//蓝哨兵HP
	
	
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_1map_x;// 己方 1 号英雄机器人位置 x 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_1map_y; // 己方 1 号英雄机器人位置 y 轴坐标
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_2map_x;// 己方 2 号工程机器人位置 x 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_2map_y;// 己方 2 号工程机器人位置 y 轴坐标
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_3map_x;// 己方 3 号步兵机器人位置 x 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_3map_y;	// 己方 3 号步兵机器人位置 y 轴坐标
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_4map_x;// 己方 4 号步兵机器人位置 y 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_4map_y;// 己方 4 号步兵机器人位置 y 轴坐标
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_5map_x;// 己方 5 号步兵机器人位置 y 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_5map_y;// 己方 5 号步兵机器人位置 y 轴坐标
  union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_7map_x;// 己方 7 号哨兵机器人位置 x 轴坐标
	union
	{
		uint8_t buff[4];
		float value_offset;
	}standard_7map_y;// 己方 7 号哨兵机器人位置 y 轴坐标
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}red_outpost_HP; // 红方前哨站血量
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}red_base_HP; // 红方基地血量
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}blue_outpost_HP;// 蓝方前哨站血量
	
	union
	{
		uint8_t buff[4];
		float value_offset;
	}blue_base_HP;// 蓝方基地血量
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}left_real_heat;//左枪管实时热量
	
	union
	{
		uint8_t buff[4];
		int value_offset;
	}right_real_heat;//右枪管实时热量
	
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


