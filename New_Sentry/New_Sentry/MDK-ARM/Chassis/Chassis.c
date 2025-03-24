                                                                                                           #include "Chassis.h"
#include "cmsis_os.h"
#include "MotorCtrl.h"
#include "MyLib.h"
#include "dbus.h"
#include "structs.h"
#include "user_define.h"
#include "Gimbal.h"
#include "vision_can.h"
#include "judge.h"
#include "vision.h"
#include "protect.h"

Chassis_Type Chassis_Data;

void speed_cal(float vx, float vy, float vw,float angle, float *motor_speed_1, float *motor_speed_2, float *motor_speed_3, float *motor_speed_4);

PidTypeDef Chassis_Follow_PID;

float Chassis_Follow_pid[3]={0.0050000005,0,0.0010000005};


/**
  * @brief  ������ģʽѡ��
  * @param  void
  * @retval ��ȡdbus�����жϻ�����ģʽ
  * @attention  ��
  */
void Robot_Mode_Select(void)
{
	
	if(RC_Ctl.rc.s1 == 1 && RC_Ctl.rc.s2 == 2)
	{
		Chassis_Data.RobotData.robot_mode = AUTO_MODE;     
	}	
	else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 2 )
	{
		Chassis_Data.RobotData.robot_mode = FOLLOW_MODE;   // ���̸���ģʽ
	}
	else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 3 )
	{
		Chassis_Data.RobotData.robot_mode = DIFFSPINNING;   // С���ݷ�תģʽ
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 3)
	{
		Chassis_Data.RobotData.robot_mode = UNFOLLOW_MODE;  // ������
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1)
	{
		Chassis_Data.RobotData.robot_mode = SHOOT_MODE;	    //����Ħ����
	}
	else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2){
		Chassis_Data.RobotData.robot_mode = SPINNING;		   // С����
	}	
	else
	{
//		soft_reset();
		Chassis_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE;
	}
	
}
float temp1,temp2,mintemp1;

float angle_match(void)//�ҵ���Ҫ��ת����С�Ƕ�
{                       //��ǰ�Ƕȼ�ȥĿ��Ƕ�    
	
	temp1 = Motor_4310.angle - GIMBLE_YAW_MOTOR_ANGLE;
  temp2 = Motor_4310.angle - GIMBLE_YAW_MOTOR_ANGLE_FU;
    
	if(temp1 > 0.5f*_PI)
     temp1 -= _PI;
  else if(temp1 < -0.5f*_PI)
     temp1 += _PI;
  if(temp2 > 0.5f*_PI)
     temp2 -= _PI;
  else if(temp2 < -0.5f*_PI)
        temp2 += _PI;
  mintemp1 = (fabs(temp1) < fabs(temp2) ? temp1 : temp2);
  return mintemp1;
}




void StartChassisTask(void const * argument)
{
	
	portTickType currentTime;
	//Init
//	Chassis_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE ;
	pid_param_init(&Chassis_Follow_PID, PID_POSITION, Chassis_Follow_pid, 0.08,     0.001,   1e30, 0.005,       0.1, 0,  0);
	while(1)
	{  
		currentTime = xTaskGetTickCount(); //��ǰϵͳʱ��
	  Robot_Mode_Select();
//		Gimbal_Data.yaw_angle_target = RC_Ctl.rc.ch2 / 5.0f;
			if(Chassis_Data.RobotData.robot_mode == SPINNING)
			{
				Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
				//ң������ֵ
				float temp_yaw_angle_target = Gimbal_Data.yaw_angle_target - 0.0001f * RC_Ctl.rc.ch2;
				Gimbal_Data.yaw_angle_target = __normalangle(&temp_yaw_angle_target, -180, 180);
				Chassis_Data.vx_target = (RC_Ctl.rc.ch1)/660.f;
				Chassis_Data.vy_target = (RC_Ctl.rc.ch0)/660.f;
				Chassis_Data.vw_target = 0.005f;

				//�ٶȽ���
				speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target); 
				Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
				Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
				Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
				Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
			}
			else if(Chassis_Data.RobotData.robot_mode == UNFOLLOW_MODE)
			{
				Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
				//ң������ֵ
				float temp_yaw_angle_target = Gimbal_Data.yaw_angle_target - 0.0001f * RC_Ctl.rc.ch2;
				Gimbal_Data.yaw_angle_target = __normalangle(&temp_yaw_angle_target, -180, 180);
				Chassis_Data.vx_target = (RC_Ctl.rc.ch1)/660.f;
				Chassis_Data.vy_target = (RC_Ctl.rc.ch0)/660.f;
				Chassis_Data.vw_target = 0;
				//�ٶȽ���
//				speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target); 
//				Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
//				Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
//				Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
//				Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
				
				Motor_3508[0].speed_rpm_target = Chassis_Data.vx_target*60.0f/(WHEEL_PERIMETER*3.14f)*CHASSIS_DECELE_RATIO*3000;
				Motor_3508[1].speed_rpm_target = -Chassis_Data.vy_target*60.0f/(WHEEL_PERIMETER*3.14f)*CHASSIS_DECELE_RATIO*3000;
				Motor_3508[2].speed_rpm_target = -Chassis_Data.vx_target*60.0f/(WHEEL_PERIMETER*3.14f)*CHASSIS_DECELE_RATIO*3000;
				Motor_3508[3].speed_rpm_target = Chassis_Data.vy_target*60.0f/(WHEEL_PERIMETER*3.14f)*CHASSIS_DECELE_RATIO*3000;
			}
			else if(Chassis_Data.RobotData.robot_mode == FOLLOW_MODE)
			{
				Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
				//ң������ֵ
				float temp_yaw_angle_target = Gimbal_Data.yaw_angle_target - 0.0001f * RC_Ctl.rc.ch2;
				Gimbal_Data.yaw_angle_target = __normalangle(&temp_yaw_angle_target, -180, 180);
				Chassis_Data.vx_target = (RC_Ctl.rc.ch1)/200.f;
				Chassis_Data.vy_target = (RC_Ctl.rc.ch0)/200.f;
				angle_match();
				pid_caculate(&Chassis_Follow_PID,mintemp1,0);
				Chassis_Data.vw_target = -Chassis_Follow_PID.out;
				//�ٶȽ���
				speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target); 
				Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
				Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
				Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
				Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
			}
			else if(Chassis_Data.RobotData.robot_mode == AUTO_MODE)
			{
				Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
				float temp_yaw_angle_target = Gimbal_Data.yaw_angle_target;
				Gimbal_Data.yaw_angle_target = __normalangle(&temp_yaw_angle_target, -180, 180);
				Chassis_Data.vx_target = VisionValue.vision_absolute_vx.value;
				Chassis_Data.vy_target = -VisionValue.vision_absolute_vy.value;
				Chassis_Data.vw_target = 0;
			if(get_outpost_HP()<=0 && judge_frame_rx.data.game_status.game_progress == 4 && VisionValue.vision_absolute_vx.value == 0 && VisionValue.vision_absolute_vy.value)
			{
				Chassis_Data.vw_target = 0.007f;
			}
			else if(get_outpost_HP()<=0 && judge_frame_rx.data.game_status.game_progress == 4)
			{
				Chassis_Data.vw_target = 0.004f;
			}
				//�ٶȽ���
				speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target); 
				Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
				Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
				Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
				Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
			}
			else if(Chassis_Data.RobotData.robot_mode == DIFFSPINNING)
			{
				Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
				//ң������ֵ
				float temp_yaw_angle_target = Gimbal_Data.yaw_angle_target ;
				Gimbal_Data.yaw_angle_target = __normalangle(&temp_yaw_angle_target, -180, 180);
				Chassis_Data.vx_target = Gimbal_Data.Speed_Vx;
				Chassis_Data.vy_target = Gimbal_Data.Speed_Vy;
				Chassis_Data.vw_target = -0.005f;

				//�ٶȽ���
				speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target); 
				Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
				Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
				Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
				Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
			}
		  else
		  {
			  Chassis_Data.angle_yaw_error = GIMBLE_YAW_MOTOR_ANGLE - Motor_4310.angle;
			  Chassis_Data.vx_target = (RC_Ctl.rc.ch2)/660.f;
				Chassis_Data.vy_target = (RC_Ctl.rc.ch3)/660.f;
			  Chassis_Data.vx_target = Chassis_Data.vx_target/4;
				Chassis_Data.vy_target = Chassis_Data.vy_target/4;
			  speed_cal(Chassis_Data.vx_target, Chassis_Data.vy_target, Chassis_Data.vw_target, Chassis_Data.angle_yaw_error, &Chassis_Data.motor1_speed_target, &Chassis_Data.motor2_speed_target, &Chassis_Data.motor3_speed_target, &Chassis_Data.motor4_speed_target);
			  Motor_3508[0].speed_rpm_target = Chassis_Data.motor1_speed_target;
			  Motor_3508[1].speed_rpm_target = Chassis_Data.motor2_speed_target;
			  Motor_3508[2].speed_rpm_target = Chassis_Data.motor3_speed_target;
			  Motor_3508[3].speed_rpm_target = Chassis_Data.motor4_speed_target;
		  }
		vTaskDelayUntil(&currentTime, 1); //������ʱ
	}
}




void speed_cal(float vx, float vy, float vw,float angle, float *motor_speed_1, float *motor_speed_2, float *motor_speed_3, float *motor_speed_4)
{
	//������
	float rad_angle;
	float temp_vx, temp_vy, temp_vw;
	rad_angle = angle;
	temp_vw=vw;
	temp_vx=vx*cos(rad_angle)+vy*sin(rad_angle);
	temp_vy=vx*sin(rad_angle)-vy*cos(rad_angle);
	
	float wheel_rpm_ratio;
  wheel_rpm_ratio = 60.0f/(WHEEL_PERIMETER*3.14f)*CHASSIS_DECELE_RATIO*1000;
	//wheel_rpm_ratio���ڰ��ٶ�ת��Ϊ���ת�٣�60.0f�Ƿ��ӵ���仯
	//WHEEL_PERIMETER�����ӵ��ܳ�
	//
	*motor_speed_1 = (   temp_vx - temp_vy + temp_vw * (LENGTH_A+LENGTH_B))*wheel_rpm_ratio;//x��y�����ٶ�,w����ת���ٶ�
  *motor_speed_2 = (   temp_vx + temp_vy + temp_vw * (LENGTH_A+LENGTH_B))*wheel_rpm_ratio;
  *motor_speed_3 = (  -temp_vx + temp_vy + temp_vw * (LENGTH_A+LENGTH_B))*wheel_rpm_ratio;
  *motor_speed_4 = (  -temp_vx - temp_vy + temp_vw * (LENGTH_A+LENGTH_B))*wheel_rpm_ratio;
	
	//�����  Vx = 0.25 * (V1 + V2 + V3 + V4)�� Vy = 0.25 * (-V1 + V2 - V3 + V4)
       
	
}


