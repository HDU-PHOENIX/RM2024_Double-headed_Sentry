//#include "protect.h"
//#include "Gimbal.h"
//#include "MotorCtrl.h"
//#include "dbus.h"
//#include "vision.h"
//#include "Chassis.h"
//#include "tim.h"
///**
//  * @brief  ��������������
//  * @param  void
//  * @retval void
//  * @attention
//  */


//void soft_reset(void)
//{
//    __set_FAULTMASK(1); //�ر������ж�
//    NVIC_SystemReset(); //��λ
//}

//int Flag = 0;//Ħ����
//int connect=0;//��λ��־
//void ShowRunningFun(void const * argument)
//{
//    portTickType currentTime;
//    for(;;)
//    {	  
//        currentTime = xTaskGetTickCount();//��ǰϵͳʱ��
//        #if (SAFE_MODE==1)
//        if(currentTime>REMOTE_ulGetLostTime()||REMOTE_IfDataError()==true)	//ң����ʧ��ʱ�����,�������ݳ���
//        {
//			    connect=1;
//			    PID_clear_all();
//          vTaskSuspend(GimbalTaskHandle);		//���������
//          vTaskSuspend(ChassisTaskHandle);
//					vTaskSuspend(MotorTaskHandle);
//          vTaskResume(OutControl_TaskHandle);//���ʧ�ر�����������
//          VisionValue.vision_absolute_vx.value = 0;
//					VisionValue.vision_absolute_vy.value = 0;
//					VisionValue.vision_YAW_value.value = 0;
//					

//		}
//        else if(connect==1)
//        {
//          vTaskResume(GimbalTaskHandle);		//�ָ�����
//          vTaskResume(ChassisTaskHandle);
//					vTaskResume(MotorTaskHandle);
//			    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//          vTaskSuspend(OutControl_TaskHandle);//����ʧ�ر�����������
//          TIM12->CCR1=0;
//          TIM12->CCR2=0;
//        }
//#endif
//				if(RC_Ctl.rc.s1 == 1 && RC_Ctl.rc.s2 == 2)
//		{
//			Chassis_Data.RobotData.robot_mode = AUTO_MODE;     
//		}	
//		else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 2 )
//		{
//			Chassis_Data.RobotData.robot_mode = FOLLOW_MODE;   // ���̸���ģʽ
//		}
//		else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 3 )
//		{
//			Chassis_Data.RobotData.robot_mode = FIX_MODE;   // ��YAW�̶�ģʽ
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 3)
//		{
//			Chassis_Data.RobotData.robot_mode = UNFOLLOW_MODE;  // ������
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1)
//		{
//			Chassis_Data.RobotData.robot_mode = SHOOT_MODE;	    //����Ħ����
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2){
//			Chassis_Data.RobotData.robot_mode = SPINNING;		   // С����
//		}	
//		else
//		{
//			Chassis_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE;
//			soft_reset();
//		}
//        vTaskDelayUntil(&currentTime, 40);//������ʱ
//    }
//}

////ʧ�ؿ��������������ÿ4msִ��һ�Σ�����ӳ�
//void OutControl_Fun(void *pvParameters)
//{
//    for(;;)
//    {
//        vTaskDelay(TIME_STAMP_4MS);				//4ms

//        //SystemValue=Starting;//ϵͳ�ָ�������״̬������Ҫ������̨����
//       //REMOTE_vResetData();//ң�����ݻָ���Ĭ��״̬

//        //�ر����е�����
//			Motor_3508_send(&CAN_CHASSIS_3508_1, 0x200, 0, 0, 0, 0);
//			Motor_4310_send(&CAN_YAW_4310,0x01,0,0,0,0,0);
//		 HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);

//		
//    }
//}

//void PID_clear_all()
//{

//  PID_clear(&PID_CHASSIS_3508_1_SPEED);
//	PID_clear(&PID_CHASSIS_3508_2_SPEED);
//	PID_clear(&PID_CHASSIS_3508_3_SPEED);
//	PID_clear(&PID_CHASSIS_3508_4_SPEED);
//	PID_clear(&PID_YAW_4310_SPEED);
//  PID_clear(&PID_YAW_4310_ANGLE);

//}

