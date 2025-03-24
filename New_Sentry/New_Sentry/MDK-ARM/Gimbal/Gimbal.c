#include "Gimbal.h"
#include "cmsis_os.h"
#include "MotorCtrl.h"
#include "MyLib.h"
#include "vision_can.h"
#include "vision.h"

Gimbal_Type Gimbal_Data;


void StartGimbalTask(void const * argument)
{
	portTickType currentTime;
	while(1)
	{
		currentTime = xTaskGetTickCount(); //当前系统时间
		vision_transmit();
		vTaskDelayUntil(&currentTime, 5); //绝对延时
	}
}
