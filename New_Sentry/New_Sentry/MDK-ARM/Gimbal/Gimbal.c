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
		currentTime = xTaskGetTickCount(); //��ǰϵͳʱ��
		vision_transmit();
		vTaskDelayUntil(&currentTime, 5); //������ʱ
	}
}
