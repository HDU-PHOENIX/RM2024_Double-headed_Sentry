#ifndef __MOTORCTRL_H__
#define __MOTORCTRL_H__

//#include "DJI6020.h"
#include "DJI3508.h"
#include "DM4310.h"
//#include "DJI2006.h"
#include "pid.h"
//3796
#define GIMBLE_YAW_MOTOR_ANGLE_FU 1.68169951f 		//����̨�����������������һ��ʱ����yaw�����Ķ�Ӧ���ԽǶ�
#define GIMBLE_YAW_MOTOR_ANGLE -1.49867404f //����̨����������̸�����һ��ʱ����yaw�����Ķ�Ӧ���ԽǶ�

#define GIMBLE_INIT_YAW_OK_ANGLE 10

extern PidTypeDef PID_CHASSIS_3508_1_SPEED;
extern PidTypeDef PID_CHASSIS_3508_2_SPEED;
extern PidTypeDef PID_CHASSIS_3508_3_SPEED;
extern PidTypeDef PID_CHASSIS_3508_4_SPEED;
extern PidTypeDef PID_YAW_4310_SPEED;
extern PidTypeDef PID_YAW_4310_ANGLE;

#endif
