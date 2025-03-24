//#include "protect.h"
//#include "Gimbal.h"
//#include "MotorCtrl.h"
//#include "dbus.h"
//#include "vision.h"
//#include "Chassis.h"
//#include "tim.h"
///**
//  * @brief  检测程序正常运行
//  * @param  void
//  * @retval void
//  * @attention
//  */


//void soft_reset(void)
//{
//    __set_FAULTMASK(1); //关闭所有中断
//    NVIC_SystemReset(); //复位
//}

//int Flag = 0;//摩擦轮
//int connect=0;//复位标志
//void ShowRunningFun(void const * argument)
//{
//    portTickType currentTime;
//    for(;;)
//    {	  
//        currentTime = xTaskGetTickCount();//当前系统时间
//        #if (SAFE_MODE==1)
//        if(currentTime>REMOTE_ulGetLostTime()||REMOTE_IfDataError()==true)	//遥控器失联时间过长,或者数据出错
//        {
//			    connect=1;
//			    PID_clear_all();
//          vTaskSuspend(GimbalTaskHandle);		//将任务挂起
//          vTaskSuspend(ChassisTaskHandle);
//					vTaskSuspend(MotorTaskHandle);
//          vTaskResume(OutControl_TaskHandle);//解挂失控保护控制任务
//          VisionValue.vision_left_pitch_value.value = 0;
//					VisionValue.vision_left_yaw_value.value = 0;
//					VisionValue.Left_center_flag = '0';
//					

//		}
//        else if(connect==1)
//        {
//          vTaskResume(GimbalTaskHandle);		//恢复任务
//          vTaskResume(ChassisTaskHandle);
//					vTaskResume(MotorTaskHandle);
//			    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//          vTaskSuspend(OutControl_TaskHandle);//挂起失控保护控制任务
//          TIM12->CCR1=0;
//          TIM12->CCR2=0;
//        }
//#endif
//				if(RC_Ctl.rc.s1 == 1 && RC_Ctl.rc.s2 == 2)
//		{
//			Gimbal_Data.RobotData.robot_mode = AUTO_MODE;     
//		}	
//		else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 2 )
//		{
//			Gimbal_Data.RobotData.robot_mode = FOLLOW_MODE;   // 底盘跟随模式
//		}
//		else if(RC_Ctl.rc.s1 == 2 && RC_Ctl.rc.s2 == 3 )
//		{
//			Gimbal_Data.RobotData.robot_mode = FIX_MODE;   // 大YAW固定模式
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 3)
//		{
//			Gimbal_Data.RobotData.robot_mode = UNFOLLOW_MODE;  // 不跟随
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 1)
//		{
//			Gimbal_Data.RobotData.robot_mode = SHOOT_MODE;	    //开启摩擦轮
//		}
//		else if(RC_Ctl.rc.s1 == 3 && RC_Ctl.rc.s2 == 2){
//			Gimbal_Data.RobotData.robot_mode = SPINNING;		   // 小陀螺
//		}	
//		else
//		{
//			Gimbal_Data.RobotData.robot_mode = ALL_POWER_OFF_MODE;
//			soft_reset();
//		}
//        vTaskDelayUntil(&currentTime, 40);//绝对延时
//    }
//}

////失控控制任务若解挂则每4ms执行一次，相对延迟
//void OutControl_Fun(void *pvParameters)
//{
//    for(;;)
//    {
//        vTaskDelay(TIME_STAMP_4MS);				//4ms

//        //SystemValue=Starting;//系统恢复至重启状态，最重要的是云台重启
//       //REMOTE_vResetData();//遥控数据恢复至默认状态

//        //关闭所有电机输出
//			Motor_4310_send(&CAN_PITCH_4310,0x02,0,0,0,0,0);
//			Motor_6020_send(&CAN_GIMBAL_6020, 0x1FF,0,0,0,0);
//			Motor_3508_send(&CAN_GIMBAL_3508_1, 0x200,0,0,0,0);

//		
//    }
//}

//void PID_clear_all()
//{

//  PID_clear(&PID_GIMBAL_3508_1_SPEED);
//	PID_clear(&PID_GIMBAL_3508_2_SPEED);
//	PID_clear(&PID_GIMBAL_2006_SPEED);
//	PID_clear(&PID_GIMBAL_6020_SPEED);
//	PID_clear(&PID_GIMBAL_6020_ANGLE);
//  PID_clear(&PID_PITCH_4310_SPEED);
//	PID_clear(&PID_PITCH_4310_SPEED);
//	PID_clear(&PID_VISION_YAW_ANGLE);
//	PID_clear(&PID_VISION_PITCH_ANGLE);

//}

