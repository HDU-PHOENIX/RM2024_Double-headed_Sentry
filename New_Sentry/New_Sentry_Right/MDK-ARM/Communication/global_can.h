#include "can.h"

//µç»úID
#define CANID_GIMBAL_3508_1 0x201
#define CANID_GIMBAL_3508_2 0x202
#define CANID_GIMBAL_6020 0x208
#define CANID_GIMBAL_2006 0x203
#define CANID_PITCH_4310 0x0D


#define CAN_GIMBAL_3508_1 hcan1
#define CAN_GIMBAL_3508_2 hcan1
#define CAN_GIMBAL_6020 hcan1
#define CAN_GIMBAL_2006 hcan1
#define CAN_PITCH_4310 hcan2

void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan);
void CAN_Init(void);
