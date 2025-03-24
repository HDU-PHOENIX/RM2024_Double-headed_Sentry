#include "can.h"

//���ID
#define CANID_CHASSIS_3508_1 0x201
#define CANID_CHASSIS_3508_2 0x202
#define CANID_CHASSIS_3508_3 0x203
#define CANID_CHASSIS_3508_4 0x204
#define CANID_YAW_4310 0x0B

//ң����
#define CANID_CHASSIS_C	0x101
#define CANID_CHASSIS_C_1	0x102


//����ϵͳ
#define CANID_game_robot_status 0x103
#define CANID_power_heat_data		0x104
#define CANID_shoot_data				0x105

//�Ӿ�
#define CANID_VISION_C_1 0x106 //�����ٶ�
#define CANID_VISION_C_2 0x107 //YAW
#define CANID_VISION_C_3 0x108 //IMU
#define CANID_VISION_C_4 0x109 //��ͷ����ֵ
#define CANID_VISION_C_5 0x10A //��ͷ�����־λ

#define CAN_CHASSIS_3508_1 hcan1
#define CAN_CHASSIS_3508_2 hcan1
#define CAN_CHASSIS_3508_3 hcan1
#define CAN_CHASSIS_3508_4 hcan1
#define CAN_YAW_4310 hcan2
#define CAN_CHASSIS_C hcan2
#define CAN_VISION_C hcan2
#define CAN_game_robot_status hcan2
#define CAN_power_heat_data		hcan2
#define CAN_shoot_data				hcan2

void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan);
void CAN_Init(void);
