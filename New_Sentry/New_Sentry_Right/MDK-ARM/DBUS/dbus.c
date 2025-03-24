#include "dbus.h"
#include "dma.h"
#include "usart.h"
//#include "GimbleTask.h"

RC_Ctl_t RC_Ctl;   							//声明遥控器数据结构体
uint8_t sbus_rx_buffer[18]; 		//声明遥控器缓存数组

uint8_t dbus_connect = 0;               //声明一个变量

void DBUS_Init(void)
{
	HAL_UART_Receive_DMA(&huart3,sbus_rx_buffer,18);
}

#define DBUS_HUART       huart3 /* for dji remote controler reciever */
#define DBUS_MAX_LEN     (50)
#define DBUS_BUFLEN      (18)
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) 
//{
//	if (UartHandle == &DBUS_HUART)
//	{
//		dbus_read();
//	}
//}

int uart_receive_dma_no_it(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t Size)
{
    uint32_t tmp = 0;

    tmp = huart->RxState;
    if (tmp == HAL_UART_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(huart);

        huart->pRxBuffPtr = pData;
        huart->RxXferSize = Size;

        huart->ErrorCode = HAL_UART_ERROR_NONE;

        /* Enable the DMA Stream */
        HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR,
                      (uint32_t)pData, Size);

        /* Enable the DMA transfer for the receiver request by setting the DMAR bit
        in the UART CR3 register */
        SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

        /* Process Unlocked */
        __HAL_UNLOCK(huart);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

uint8_t   ReadJY901Success = 0;	//0:未接收数据 1：成功接收数据 255：接收错误数据
static void uart_rx_idle_callback(UART_HandleTypeDef *huart)
{
    /* clear idle it flag avoid idle interrupt all the time */
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    /* handle received data in idle interrupt */
    if (huart == &DBUS_HUART)
    {
        /* clear DMA transfer complete flag */
        __HAL_DMA_DISABLE(huart->hdmarx);

        /* handle dbus data dbus_buf from DMA */
        //判断读到的数据是不是18个字节，如果是十八个字节进入回调函数解析
        //		ReadJY901Success=huart->hdmarx->Instance->NDTR;

        if ((DBUS_MAX_LEN - huart->hdmarx->Instance->NDTR) == DBUS_BUFLEN)
        {
            dbus_read();
        }

        /* restart dma transmission */
        __HAL_DMA_SET_COUNTER(huart->hdmarx, DBUS_MAX_LEN);
        __HAL_DMA_ENABLE(huart->hdmarx);

    }
}

void uart_receive_handler(UART_HandleTypeDef *huart)
{
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) &&
            __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE))
    {

        uart_rx_idle_callback(huart);
    }
}

void dbus_uart_init(void)
{
    /* open uart idle it */


    uart_receive_dma_no_it(&DBUS_HUART, sbus_rx_buffer, DBUS_MAX_LEN);

    __HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
    __HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);
}

//遥控数据混乱
uint8_t REMOTE_IfDataError( void )
{
    if ( (RC_Ctl.rc.s1 != RC_SW_UP && RC_Ctl.rc.s1 != RC_SW_MID && RC_Ctl.rc.s1 != RC_SW_DOWN)
            || (RC_Ctl.rc.s2 != RC_SW_UP && RC_Ctl.rc.s2 != RC_SW_MID && RC_Ctl.rc.s2 != RC_SW_DOWN)
            || (RC_Ctl.rc.ch0 > RC_CH_VALUE_MAX || RC_Ctl.rc.ch0 < RC_CH_VALUE_MIN)
            || (RC_Ctl.rc.ch1 > RC_CH_VALUE_MAX || RC_Ctl.rc.ch1 < RC_CH_VALUE_MIN)
            || (RC_Ctl.rc.ch2 > RC_CH_VALUE_MAX || RC_Ctl.rc.ch2 < RC_CH_VALUE_MIN)
            || (RC_Ctl.rc.ch3 > RC_CH_VALUE_MAX || RC_Ctl.rc.ch3 < RC_CH_VALUE_MIN) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void dbus_read(void)
{
	RC_Ctl.rc.ch0 = (sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8)) & 0x07ff; 
	RC_Ctl.rc.ch0-=1024;
	RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff; 
	RC_Ctl.rc.ch1-=1024;
	RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10)) & 0x07ff;
	RC_Ctl.rc.ch2-=1024;
	RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff;
	RC_Ctl.rc.ch3-=1024;	
	
	//摇杆防抖
	if(RC_Ctl.rc.ch0 <= 5 && RC_Ctl.rc.ch0 >= -5)
			RC_Ctl.rc.ch0 = 0;
	if(RC_Ctl.rc.ch1 <= 5 && RC_Ctl.rc.ch1 >= -5)
			RC_Ctl.rc.ch1 = 0;
	if(RC_Ctl.rc.ch2 <= 5 && RC_Ctl.rc.ch2 >= -5)
			RC_Ctl.rc.ch2 = 0;
	if(RC_Ctl.rc.ch3 <= 5 && RC_Ctl.rc.ch3 >= -5)
			RC_Ctl.rc.ch3 = 0;
	
	

	RC_Ctl.rc.s1  = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;                           
	RC_Ctl.rc.s2  = ((sbus_rx_buffer[5] >> 4)& 0x0003);    
	RC_Ctl.rc.wheel = (sbus_rx_buffer[16] | sbus_rx_buffer[17] << 8) - 1024;
	
	RC_Ctl.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8);                    //!< Mouse X axis        
//	if(RC_Ctl.mouse.x > 150)
//		RC_Ctl.mouse.x = 150;
//	if(RC_Ctl.mouse.x < -150)
//		RC_Ctl.mouse.x = -150;
	RC_Ctl.mouse.y = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8);                    //!< Mouse Y axis      
	RC_Ctl.mouse.z = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8);                  //!< Mouse Z axis 
	if(RC_Ctl.mouse.x <= 1 && RC_Ctl.mouse.x >= -1)
		RC_Ctl.mouse.x = 0;
	if(RC_Ctl.mouse.y <= 1 && RC_Ctl.mouse.y >= -1)
		RC_Ctl.mouse.y = 0;	
	RC_Ctl.mouse.press_l = sbus_rx_buffer[12];                                        //!< Mouse Left Is Press      
	RC_Ctl.mouse.press_r = sbus_rx_buffer[13];                                        //!< Mouse Right Is Press 
	RC_Ctl.key.v = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8);   			//!< KeyBoard value
	
	RC_Ctl.misstimeout = 0;	//超时时间清零
}





