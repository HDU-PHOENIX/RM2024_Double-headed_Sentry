#include "MotorCtrl.h"
#include "cmsis_os.h"
#include "MyLib.h"
#include "dbus.h"
#include "kalman.h"
#include "MyLib.h"
#include "Gimbal.h"
#include "INS_task.h"
#include "Chassis.h"
#include "user_define.h"
#include "judge.h"
#include "pid.h"

PidTypeDef PID_CHASSIS_3508_1_SPEED;
PidTypeDef PID_CHASSIS_3508_2_SPEED;
PidTypeDef PID_CHASSIS_3508_3_SPEED;
PidTypeDef PID_CHASSIS_3508_4_SPEED;
PidTypeDef PID_YAW_4310_SPEED;
PidTypeDef PID_YAW_4310_ANGLE;

float PID_CHASSIS_3508_1_SPEED_K[3] = {6,0.5,0};
float PID_CHASSIS_3508_2_SPEED_K[3] = {6,0.5,0};
float PID_CHASSIS_3508_3_SPEED_K[3] = {6,0.5,0};
float PID_CHASSIS_3508_4_SPEED_K[3] = {6,0.5,0};
float PID_YAW_4310_SPEED_K[3] = {0,0,0};
float PID_YAW_4310_ANGLE_K[3] = {1.1,0.0025,50};

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
void Handle_Angle360_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 180.0f)    //PI ：半圈机械角度
	{
		*cur += 360.0f;        //2PI:整圈
	}
	else if(*tar - *cur < -180.0f)
	{
		*cur = *cur - 360.0f;
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

#define  CHAS_CURRENT_LIMIT        40000    //四个轮子的速度总和最大值,单个输出*4,限功率调比例可用
float fChasCurrentLimit = CHAS_CURRENT_LIMIT;//限制4个轮子的速度总和
float fTotalCurrentLimit;//电流分配,平地模式下分配是均匀的
float WARNING_REMAIN_POWER = 42;//裁判系统剩余焦耳能量低于这个数值则开始限功率,40扭屁股会超功率,平地开不会超
float chassis_totaloutput;
float Power_Limit_Gain = 1.0f;
float Joule_Residue = 0;//剩余焦耳缓冲能量
int16_t judgDataCorrect = 0;//裁判系统数据是否可用
static int32_t judgDataError_Time = 0;

void Motor_Ctrl(void)
{	
	pid_caculate(&PID_CHASSIS_3508_1_SPEED,Motor_3508[0].speed_rpm,Motor_3508[0].speed_rpm_target);
	pid_caculate(&PID_CHASSIS_3508_2_SPEED,Motor_3508[1].speed_rpm,Motor_3508[1].speed_rpm_target);
	pid_caculate(&PID_CHASSIS_3508_3_SPEED,Motor_3508[2].speed_rpm,Motor_3508[2].speed_rpm_target);
	pid_caculate(&PID_CHASSIS_3508_4_SPEED,Motor_3508[3].speed_rpm,Motor_3508[3].speed_rpm_target);
	
	/*********************限功率部分*************************/
	judgDataCorrect = JUDGE_sGetDataState();//裁判系统数据是否可用
	Joule_Residue = JUDGE_fGetRemainEnergy();//剩余焦耳能量
	//统计底盘总输出
	chassis_totaloutput=fabsf(PID_CHASSIS_3508_1_SPEED.out)+fabsf(PID_CHASSIS_3508_2_SPEED.out)
											+fabsf(PID_CHASSIS_3508_3_SPEED.out)+fabsf(PID_CHASSIS_3508_4_SPEED.out);
	if(judgDataCorrect ==0)
	{   
		//裁判系统无效时强制限速
		judgDataError_Time++;
		if(judgDataError_Time > 100)
		{
				fTotalCurrentLimit = fChasCurrentLimit/4;//降为最大的1/4
		}
	}
	else 
	{
		judgDataError_Time = 0;
		//剩余焦耳量过小,开始限制输出,限制系数为平方关系
		if(Joule_Residue < WARNING_REMAIN_POWER) {
				Power_Limit_Gain = (float)(Joule_Residue / WARNING_REMAIN_POWER)
								 * (float)(Joule_Residue / WARNING_REMAIN_POWER);
				fTotalCurrentLimit = Power_Limit_Gain * fChasCurrentLimit;
		}
		else {  //焦耳能量恢复到一定数值
				fTotalCurrentLimit = fChasCurrentLimit;
		}
	}
	//底盘各电机电流重新分配
	if (chassis_totaloutput > fTotalCurrentLimit ) {//可以拿一个变量中转,防止out被改后出现的pid问题
			PID_CHASSIS_3508_1_SPEED.out= (float)(PID_CHASSIS_3508_1_SPEED.out / chassis_totaloutput * fTotalCurrentLimit);
			PID_CHASSIS_3508_2_SPEED.out= (float)(PID_CHASSIS_3508_2_SPEED.out / chassis_totaloutput * fTotalCurrentLimit);
			PID_CHASSIS_3508_3_SPEED.out= (float)(PID_CHASSIS_3508_3_SPEED.out / chassis_totaloutput * fTotalCurrentLimit);
			PID_CHASSIS_3508_4_SPEED.out= (float)(PID_CHASSIS_3508_4_SPEED.out / chassis_totaloutput * fTotalCurrentLimit);
	}
	/******************************************************/
	Motor_3508_send(&CAN_CHASSIS_3508_1, 0x200, PID_CHASSIS_3508_1_SPEED.out, PID_CHASSIS_3508_2_SPEED.out, PID_CHASSIS_3508_3_SPEED.out, PID_CHASSIS_3508_4_SPEED.out);
	
	float temp_yaw_angle = Gimbal_Data.yaw_angle;
	Handle_Angle360_PID_Over_Zero(&Gimbal_Data.yaw_angle_target, &temp_yaw_angle);
	
	pid_caculate(&PID_YAW_4310_ANGLE, temp_yaw_angle, Gimbal_Data.yaw_angle_target);
	pid_caculate(&PID_YAW_4310_SPEED, Motor_4310.speed_rpm, Motor_4310.speed_rpm_target);
	
	if(Motor_4310.msg_frequent < 900)
	{
		Motor_4310_send(&CAN_YAW_4310,0x01,0,0,0,0,0);
	}
	else
	{
		Motor_4310_send(&CAN_YAW_4310,0x01,0,0,0,0,-PID_YAW_4310_ANGLE.out);
	}
}

void Motor_PID_Init(void)
{

	pid_param_init(&PID_CHASSIS_3508_1_SPEED,PID_DELTA,PID_CHASSIS_3508_1_SPEED_K,  2700,      2000,  3e38,  0,  0.1, 0, 0);
	
  pid_param_init(&PID_CHASSIS_3508_2_SPEED,PID_DELTA,PID_CHASSIS_3508_2_SPEED_K,  2500,      2000,  3e38,  0,  0.1, 0, 0);
	
	pid_param_init(&PID_CHASSIS_3508_3_SPEED,PID_DELTA,PID_CHASSIS_3508_3_SPEED_K,  2700,      2000,  3e38,  0,  0.1, 0, 0);
	
	pid_param_init(&PID_CHASSIS_3508_4_SPEED,PID_DELTA,PID_CHASSIS_3508_4_SPEED_K,  2500,      2000,  3e38,  0,  0.1, 0, 0);
	
	pid_param_init(&PID_YAW_4310_SPEED,PID_POSITION,PID_YAW_4310_SPEED_K,  5, 3, 200, 0, 0.1, 0, 0);
	
	pid_param_init(&PID_YAW_4310_ANGLE,PID_POSITION,PID_YAW_4310_ANGLE_K,  5, 3, 200, 0, 0.1, 0, 0);
	
	Motor_4310_start(&CAN_YAW_4310, 0x01);
}
//StartChassisTask
//云台电机控制任务，将所有电机速度环、位置环都在这里运行，其他地方只需要改电机参数目标值就行了
void StartMotorTask(void const * argument)
{
	portTickType currentTime;
	Motor_PID_Init();
	for(;;)
	{
		currentTime = xTaskGetTickCount(); //当前系统时间
		if(Chassis_Data.RobotData.robot_mode == ALL_POWER_OFF_MODE)
		{
			//Motor_Ctrl();
			Motor_3508_send(&CAN_CHASSIS_3508_1, 0x200, 0, 0, 0, 0);
			//Motor_3508_send(&hcan1, 0x200, PID_CHASSIS_3508_1_SPEED.out, PID_CHASSIS_3508_2_SPEED.out, PID_CHASSIS_3508_3_SPEED.out, PID_CHASSIS_3508_4_SPEED.out);
			Motor_4310_send(&CAN_YAW_4310,0x01,0,0,0,0,0);
		}
		else
		{
			Motor_Ctrl();
			if(Motor_4310.dm_err == 0 )//|| Motor_4310.hall == 0
			{	//电机使能
				Motor_4310_start(&CAN_YAW_4310,0x01);
			}
		}
		vTaskDelayUntil(&currentTime, 1); //绝对延时
	}
}
