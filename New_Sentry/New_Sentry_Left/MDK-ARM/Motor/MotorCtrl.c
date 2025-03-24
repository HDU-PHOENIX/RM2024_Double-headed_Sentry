#include "MotorCtrl.h"
#include "cmsis_os.h"
#include "MyLib.h"
#include "dbus.h"
#include "kalman.h"
#include "MyLib.h"
#include "Gimbal.h"
#include "INS_task.h"
#include "myFilter.h"

PidTypeDef PID_GIMBAL_3508_1_SPEED;
PidTypeDef PID_GIMBAL_3508_2_SPEED;
PidTypeDef PID_GIMBAL_2006_SPEED;
PidTypeDef PID_GIMBAL_6020_SPEED;
PidTypeDef PID_GIMBAL_6020_ANGLE;
PidTypeDef PID_PITCH_4310_SPEED;
PidTypeDef PID_PITCH_4310_ANGLE;


float PID_GIMBAL_3508_1_SPEED_K[3] = {10,0.5,2};
float PID_GIMBAL_3508_2_SPEED_K[3] = {10,0.5,2};
float PID_GIMBAL_2006_SPEED_K[3] = {5,0.5,0};
float PID_GIMBAL_6020_SPEED_K[3] = {75,1.25,0.5};//130,0.6,0
float PID_GIMBAL_6020_ANGLE_K[3] = {0.75,0.000300000014,11.5};//0.4,0.0003,7.8
float PID_PITCH_4310_SPEED_K[3] = {0.699999988,0.00999999978,0.200000003};
float PID_PITCH_4310_ANGLE_K[3] = {11,0.25,5};//8,0,6	

float Div1 = 8000;      //衰减系数
float k1,k2;
float Div2 = 320;

/* 角度Pid时，在更新tar和cur之后紧接着调用, 处理完再进行PID计算*/
void Handle_AnglePI_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > _PI)    //PI ：半圈机械角度
	{
		*cur += _2PI;        //2PI:整圈
	}
	else if(*tar - *cur < -_PI)
	{
		*cur = *cur - _2PI;
	}
	else
	{
		//*cur = *cur;
		// do nothing
	}
}
void Handle_Angle8192_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 4096)    //4096 ：半圈机械角度
	{
		*cur += 8192;        //8192:整圈
	}
	else if(*tar - *cur < -4096)
	{
		*cur = *cur - 8192;
	}
	else
	{
		//*cur = *cur;
		// do nothing
	}
}

float yawspeed;
float yaw_angle;
float yaw_angle_target;
float pitchsee;
void Motor_Ctrl(void)
{
	
	pid_caculate(&PID_GIMBAL_3508_1_SPEED, Motor_3508[0].speed_rpm, Motor_3508[0].speed_rpm_target);
	pid_caculate(&PID_GIMBAL_3508_2_SPEED, Motor_3508[1].speed_rpm, Motor_3508[1].speed_rpm_target);
	pid_caculate(&PID_GIMBAL_2006_SPEED, Motor_2006[0].speed_rpm, Motor_2006[0].speed_rpm_target);			//拨弹盘速度环
	Motor_3508_send(&CAN_GIMBAL_3508_1, 0x200, PID_GIMBAL_3508_1_SPEED.out, PID_GIMBAL_3508_2_SPEED.out, PID_GIMBAL_2006_SPEED.out, 0);			//两个摩擦轮3508和一个拨弹盘2006电机都在can2上

	
	yaw_angle = Motor_6020[0].angle;
	yaw_angle_target = Gimbal_Data.IMUData.yaw_target;
	
	if(Gimbal_Data.IMUData.yaw_target > 8192)
	{
		Gimbal_Data.IMUData.yaw_target -= 8192;
	}
	else if(Gimbal_Data.IMUData.yaw_target < 0)
	{
		Gimbal_Data.IMUData.yaw_target += 8192;
	}
	if(Gimbal_Data.IMUData.yaw_target > 3700 && Gimbal_Data.IMUData.yaw_target < 6400)
	{
		Gimbal_Data.IMUData.yaw_target = 2800;
	}
	k1 = 0.5f + (Motor_6020[0].angle - Motor_6020[0].last_angle) / Div1;
	yaw_angle = Motor_6020[0].last_angle * k1 + (1 - k1) * Motor_6020[0].angle;
	pid_caculate(&PID_GIMBAL_6020_ANGLE, yaw_angle, yaw_angle_target);//yaw轴电机位置环
	Motor_6020[0].speed_rpm_target = PID_GIMBAL_6020_ANGLE.out;
	k2 = 0.5f + (Motor_6020[0].speed_rpm - Motor_6020[0].last_speed_rpm) / Div2;
	float yaw_speed = Motor_6020[0].last_speed_rpm * k2 + (1 - k2) * Motor_6020[0].speed_rpm;
	Motor_6020[0].v_speed_rpm = IIR_Calc(yaw_speed,&yaw_omg_LPF);
	pid_caculate(&PID_GIMBAL_6020_SPEED, Motor_6020[0].v_speed_rpm, Motor_6020[0].speed_rpm_target);//	Motor_6020[0].speed_rpm_target//yaw轴电机速度环
	Motor_6020_send(&CAN_GIMBAL_6020, 0x1FF, 0, 0, 0, PID_GIMBAL_6020_SPEED.out);	//yaw轴6020在can1上 PID_GIMBAL_6020_SPEED.out + VISION_YAW_VFF
//	
	
	float pitch_angle = Motor_4310.angle;
	float pitch_angle_target = Gimbal_Data.IMUData.pitch_target - 0.893f;
	pid_caculate(&PID_PITCH_4310_ANGLE, pitch_angle, pitch_angle_target);	//pitch轴电机位置环
	Motor_4310.speed_rpm_target = PID_PITCH_4310_ANGLE.out;
	Motor_4310.v_speed_rpm = IIR_Calc(Motor_4310.speed_rpm_target,&Pitch_omg_LPF);
	pid_caculate(&PID_PITCH_4310_SPEED, Motor_4310.speed_rpm, Motor_4310.speed_rpm_target);		//pitch轴电机速度环
	if(Motor_4310.msg_frequent < 900)
	{
		Motor_4310_send(&CAN_PITCH_4310,0x02,0,0,0,0,0);
	}
	else
	{
	  Motor_4310_send(&CAN_PITCH_4310,0x02,0,0,0,0,PID_PITCH_4310_SPEED.out);//PID_PITCH_4310_SPEED.out
	}

//	PID_YAW_4310_ANGLE.f_cal_pid(&PID_YAW_4310_ANGLE, Gimbal_Data.yaw_angle, Gimbal_Data.yaw_angle_target);	//大yaw轴电机位置环
//	PID_YAW_4310_SPEED.f_cal_pid(&PID_YAW_4310_SPEED, Motor_4310.speed_rpm, Motor_4310.speed_rpm_target);		//大yaw轴电机速度环
//	Motor_4310_send(&CAN_YAW_4310,0x303,0,0,0,0,PID_YAW_4310_SPEED.out);
}

void Motor_PID_Init(void)
{
	
	Gimbal_Data.IMUData.yaw_target = 2800;

	pid_param_init(&PID_GIMBAL_3508_1_SPEED, PID_DELTA, PID_GIMBAL_3508_1_SPEED_K, 8000, 1000, 3e38, 0, 0.1, 0, 0);

	pid_param_init(&PID_GIMBAL_3508_2_SPEED, PID_DELTA, PID_GIMBAL_3508_2_SPEED_K, 8000, 1000, 3e38, 0, 0.1, 0, 0);

	pid_param_init(&PID_GIMBAL_2006_SPEED, PID_DELTA, PID_GIMBAL_2006_SPEED_K, 10000, 8000, 20000, 0, 0.1, 0, 0);

	pid_param_init(&PID_GIMBAL_6020_SPEED, PID_POSITION, PID_GIMBAL_6020_SPEED_K, 30000, 1000, 200, 0, 0, 0, 0);

	pid_param_init(&PID_GIMBAL_6020_ANGLE, PID_POSITION, PID_GIMBAL_6020_ANGLE_K, 300, 30, 200, 0, 0, 8192, 0);

	Motor_4310_start(&CAN_PITCH_4310, 0x02);

	pid_param_init(&PID_PITCH_4310_SPEED, PID_POSITION, PID_PITCH_4310_SPEED_K, 5, 3, 50, 0, 0.1, 0, 0);

	pid_param_init(&PID_PITCH_4310_ANGLE, PID_POSITION, PID_PITCH_4310_ANGLE_K, 5, 0.1, 50, 3, 0.899999976, 3, 0);
	
}

//云台电机控制任务，将所有电机速度环、位置环都在这里运行，其他地方只需要改电机参数目标值就行了
void StartMotorTask(void const * argument)
{
	portTickType currentTime;
	Motor_PID_Init();
	for(;;)
	{
		
		currentTime = xTaskGetTickCount(); //当前系统时间
		if(Gimbal_Data.RobotData.robot_mode == ALL_POWER_OFF_MODE)
		{
			Motor_4310_send(&CAN_PITCH_4310,0x02,0,0,0,0,0);
			Motor_6020_send(&CAN_GIMBAL_6020,0x1FF,0,0,0,0);
			Motor_3508_send(&CAN_GIMBAL_3508_1,0x200,0,0,0,0);
		}
	  else
		{
			Motor_Ctrl();
			if(Motor_4310.dm_err == 0 || Motor_4310.hall == 0)
			{	//电机使能
				Motor_4310_start(&CAN_PITCH_4310,0x02);
			}
		}
		vTaskDelayUntil(&currentTime, 1); //绝对延时
	}
}
