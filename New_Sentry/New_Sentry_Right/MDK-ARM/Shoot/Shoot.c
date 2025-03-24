#include "Shoot.h"
#include "Chassis.h"
#include "cmsis_os.h"
#include "MotorCtrl.h"
#include "MyLib.h"
#include "dbus.h"
#include "Gimbal.h"
#include "vision.h"
#include "judge.h"

#define SHOOT_SPEED_15 4574
#define SHOOT_SPEED_18 5137
#define SHOOT_SPEED_22 6400
#define SHOOT_SPEED_25 7150
#define SHOOT_SPEED_30 8000

Shoot_Type Shoot_Data;

void Ammunition_Motor_Ctrol(void);
void Friction_Motor_Ctrol(void);
void Revolver_Heat_Limit(void);

void StartShootTask(void const * argument)
{
	portTickType currentTime;
	while(1)
	{
		currentTime = xTaskGetTickCount(); //��ǰϵͳʱ��
		Ammunition_Motor_Ctrol();
		Friction_Motor_Ctrol();
		vTaskDelayUntil(&currentTime, 1); //������ʱ
	}
}



/********************�����̿��Ƴ���*********************/
float ammu_speed = SHOOT_HEAT_ENOUGH;
uint16_t  RemainingHeat;
void Ammunition_Motor_Ctrol(void)
{
		RemainingHeat = 400 - VisionValue.right_real_heat.value;
//	if(VisionValue.game_progress.value == 4)
		if(Gimbal_Data.Shoot_Data.use_vision == 1)	//������Ӿ�ģʽ
		{
			if(Gimbal_Data.Shoot_Data.vision_mode == 1)		//����
			{
				if(VisionValue.center_flag == '1')	//����������
				{
					if(VisionValue.vision_Right_Gimbal_distance.value < 8)
					{
						Motor_2006[0].speed_rpm_target = -ammu_speed * 1.4f;
					}
					else
					{
						Motor_2006[0].speed_rpm_target = -ammu_speed;
					}
				}
				else
				{
					Motor_2006[0].speed_rpm_target = 0;
				}
				if(RemainingHeat < 30)
				{
					Motor_2006[0].speed_rpm_target = 0;
				}
			}
		}
		else if((RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1) || (RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2))
		{

				if(RC_Ctl.rc.wheel >= 50 && RC_Ctl.rc.wheel <= 660)
				{
					Motor_2006[0].speed_rpm_target = ammu_speed;
				}
				else if(RC_Ctl.rc.wheel <= -50 && RC_Ctl.rc.wheel >= -660)
				{
					Motor_2006[0].speed_rpm_target = -ammu_speed;
				}
				else
				{
					Motor_2006[0].speed_rpm_target = 0;
				}
		}
}


/***********Ħ���ֿ��Ƴ���*************/
float Now_speed = 0;		//��ǰ����
float hight_speed = 7200;
float low_speed = 7000;

void Friction_Motor_Ctrol(void)
{
		if((RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1) || (Gimbal_Data.Shoot_Data.use_vision == 1))
		{
			Gimbal_Data.Shoot_Data.Friction_Motor_Speed_Set = hight_speed;
			Now_speed = VisionValue.right_bullet_speed.value;
			if(Now_speed > 29.5f)
			{
				Gimbal_Data.Shoot_Data.Friction_Motor_Speed_Set = low_speed;
			}
			Motor_3508[0].speed_rpm_target = -Gimbal_Data.Shoot_Data.Friction_Motor_Speed_Set;
			Motor_3508[1].speed_rpm_target = Gimbal_Data.Shoot_Data.Friction_Motor_Speed_Set;  
		 }
		 else
		 {
			 Motor_3508[0].speed_rpm_target = 0;
		   Motor_3508[1].speed_rpm_target = 0;
		 }
}
