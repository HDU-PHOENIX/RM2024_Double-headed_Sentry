#include "Chassis.h"
#include "cmsis_os.h"
#include "MotorCtrl.h"
#include "MyLib.h"
#include "vision_can.h"
#include "vision.h"
#include "vision_uart.h"
#include "usart.h"

Chassis_Type Chassis_Data;

void StartChassisTask(void const * argument)
{
	portTickType currentTime;
//	HAL_UART_Receive_DMA(&huart1,vision_update_rx_buff,256);
	while(1)
	{
		currentTime = xTaskGetTickCount(); //当前系统时间
//		sand_imu_data();
//		HAL_UART_Transmit(&huart1,VISION_data_sand,20,50);
//		VISION_AIM_MSG_Update(VISION_data_sand);
//		osDelay(5);
		vTaskDelayUntil(&currentTime, 1); //绝对延时
	}
}



