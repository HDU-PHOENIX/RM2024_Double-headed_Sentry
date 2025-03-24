/**
  ******************************************************************************
  * @file    tuxin.c
  * @brief   图传界面画图用于显示状态、辅助瞄准。
  * @author  CHY
  ******************************************************************************
  * @attention
  *
  * 2021.3 设计大致完成。
  * 2021.4.11 暂时还显示不了字符，只能显示图形。已经实现了图形根据车的某些状态进行动态的变化。
  * 暂时还不知道为什么会有这么诡异的事。
  *
  *
  *
  *
  ******************************************************************************
  */
#include "judge.h"
#include "tuxin.h"
#include "crc.h"
#include "string.h"
#include "cmsis_os.h"
#include "usart.h"
#include "arm_math.h"
#include "dma.h"

judge_show_data_t show_data = {0};
uint8_t Tx_buff[5][50] = {0};
uint8_t Tx_buff_Line[4][200] = {0};
uint8_t delete_buff[1][50] = {0};

//绘图段显示数据
void draw_a_cricle(uint8_t (*txbuff)[50], uint8_t i, int x, int y, uint8_t Operate_type, UART_HandleTypeDef UART)
{
    uint32_t length = 0;

    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_single_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    show_data.operate_data.show_single.grapic_data_struct.graphic_name[0] = 'r';
    show_data.operate_data.show_single.grapic_data_struct.graphic_name[1] = 'o';
    show_data.operate_data.show_single.grapic_data_struct.graphic_name[2] = 'd';
    show_data.operate_data.show_single.grapic_data_struct.operate_type = Operate_type;//图形操作
    show_data.operate_data.show_single.grapic_data_struct.graphic_type = circle;
    show_data.operate_data.show_single.grapic_data_struct.layer = i;//图层
    show_data.operate_data.show_single.grapic_data_struct.color = 1;
    show_data.operate_data.show_single.grapic_data_struct.width = 5;//线条宽度
    show_data.operate_data.show_single.grapic_data_struct.start_x = x;
    show_data.operate_data.show_single.grapic_data_struct.start_y =	y;
    show_data.operate_data.show_single.grapic_data_struct.radius = 30;//半径

    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_single.grapic_data_struct, LEN_SINGLE_GRAPH);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
    HAL_UART_Transmit(&UART, *(txbuff + i), length,5);
    memset(txbuff + i, 0, 50);
}

void Delete_All(uint8_t (*txbuff)[50],uint8_t i, UART_HandleTypeDef UART)
{
    uint32_t length = 0;
    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_delete_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);

    show_data.operate_data.client_custom_graphic_delete.operate_type = 2;
    show_data.operate_data.client_custom_graphic_delete.layer=9;

    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.client_custom_graphic_delete, 2);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
    HAL_UART_Transmit(&UART, *(txbuff + i), length,50);
    memset(txbuff + i, 0, 50);
}

void draw_a_line(uint8_t (*txbuff)[200], uint8_t i, UART_HandleTypeDef UART, uint32_t (*Data)[12])
{
    uint32_t length = 0;
		uint8_t j = 0;
    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_single_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    show_data.operate_data.show_single.grapic_data_struct.graphic_name[0] = '0';
		show_data.operate_data.show_single.grapic_data_struct.graphic_name[1] = '0';
		show_data.operate_data.show_single.grapic_data_struct.graphic_name[2] = j;
		show_data.operate_data.show_single.grapic_data_struct.operate_type = *(Data[j]+0);//Operate_type;//图形操讈E
		show_data.operate_data.show_single.grapic_data_struct.graphic_type = *(Data[j]+1);//;//图形类型
		show_data.operate_data.show_single.grapic_data_struct.layer = *(Data[j]+2);//5;//图瞾E
		show_data.operate_data.show_single.grapic_data_struct.color = *(Data[j]+3);//1;//颜色
		show_data.operate_data.show_single.grapic_data_struct.start_angle = *(Data[j]+4);//10;//
		show_data.operate_data.show_single.grapic_data_struct.end_angle = *(Data[j]+5);//10;//
		show_data.operate_data.show_single.grapic_data_struct.width = *(Data[j]+6);//10;//线条窥胰
		show_data.operate_data.show_single.grapic_data_struct.start_x = *(Data[j]+7);//x;
		show_data.operate_data.show_single.grapic_data_struct.start_y =	*(Data[j]+8);//y;
		show_data.operate_data.show_single.grapic_data_struct.radius = *(Data[j]+9);//10;//皝E?
		show_data.operate_data.show_single.grapic_data_struct.end_x = *(Data[j]+10);//10;//皝E?
		show_data.operate_data.show_single.grapic_data_struct.end_y = *(Data[j]+11);//10;//皝E?

    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_single.grapic_data_struct, LEN_SINGLE_GRAPH);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
    HAL_UART_Transmit(&UART, *(txbuff + i), length,20);
    memset(txbuff + i, 0, 200);
}

void draw_a_string(uint8_t (*txbuff)[50], uint8_t i, uint8_t str[], uint8_t len, uint16_t start_x, uint16_t start_y, uint8_t Operate_type, UART_HandleTypeDef UART)
{
    uint32_t length = 0;
    //按要求填写帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct) + 30 + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_char);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);

    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_character_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;//当前机器人id;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);//该机器人对应的客户端id
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    show_data.operate_data.show_char.grapic_data_struct.graphic_name[0] = 's';
    show_data.operate_data.show_char.grapic_data_struct.graphic_name[1] = 't';
    show_data.operate_data.show_char.grapic_data_struct.graphic_name[2] = 'r';
    show_data.operate_data.show_char.grapic_data_struct.operate_type = Operate_type;
    show_data.operate_data.show_char.grapic_data_struct.graphic_type = string;//7是字符
    show_data.operate_data.show_char.grapic_data_struct.layer = i;
    show_data.operate_data.show_char.grapic_data_struct.color = white;
    show_data.operate_data.show_char.grapic_data_struct.start_angle = 30;	//字体大小
    show_data.operate_data.show_char.grapic_data_struct.end_angle = len;	//字体长度
    show_data.operate_data.show_char.grapic_data_struct.width = 3;
    show_data.operate_data.show_char.grapic_data_struct.start_x = start_x;
    show_data.operate_data.show_char.grapic_data_struct.start_y = start_y;

    for(int y = 0; y < len; y++)
    {
        memcpy(&show_data.operate_data.show_char.data[y], str+y, 1);
    }

    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_char.grapic_data_struct, LEN_SINGLE_GRAPH + 30);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
    HAL_UART_Transmit(&UART, *(txbuff + i), length,0xffff);
    memset(txbuff + i, 0, 50);
}

void show_str(uint8_t str[],uint8_t len,uint8_t layer, uint16_t start_x,uint16_t start_y, operate_type operate, UART_HandleTypeDef UART, uint8_t width, uint8_t coller, uint8_t size)
{
	uint32_t length = 0;
	uint8_t txbuff[200] = {0};
		//按要求填写帧头
	length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_single.grapic_data_struct) + 30 + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
	show_data.frame_header.SOF = Judge_Data_SOF;
	show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_char);
	show_data.frame_header.Seq = 0;
	memcpy(txbuff,&show_data.frame_header,LEN_HEADER);
	Append_CRC8_Check_Sum(txbuff,LEN_HEADER);
	
	show_data.cmd_id = 0x0301;
	memcpy(txbuff + CMD_ID,&show_data.cmd_id,LEN_CMDID);
	show_data.student_interactive_header.data_cmd_id = client_custom_character_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;//当前机器人id;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);//该机器人对应的客户端id
	memcpy(txbuff + STU_HEADER,&show_data.student_interactive_header,LEN_STU_HEAD);
	show_data.operate_data.show_char.grapic_data_struct.graphic_name[0] = 's';
	show_data.operate_data.show_char.grapic_data_struct.graphic_name[1] = 't';
	show_data.operate_data.show_char.grapic_data_struct.graphic_name[2] = 'r';
	show_data.operate_data.show_char.grapic_data_struct.operate_type = operate;
	show_data.operate_data.show_char.grapic_data_struct.graphic_type = string;
	show_data.operate_data.show_char.grapic_data_struct.layer = layer;
	show_data.operate_data.show_char.grapic_data_struct.color = coller;
	show_data.operate_data.show_char.grapic_data_struct.start_angle = size;	//字体大小
	show_data.operate_data.show_char.grapic_data_struct.end_angle = len;	//字体长度
	show_data.operate_data.show_char.grapic_data_struct.width = width;
	show_data.operate_data.show_char.grapic_data_struct.start_x = start_x;
	show_data.operate_data.show_char.grapic_data_struct.start_y = start_y;
	
	for(int i = 0; i < len; i++)
	{
		show_data.operate_data.show_char.data[i] = str[i];
	}
	memcpy(txbuff + STU_DATA ,&show_data.operate_data.show_char.grapic_data_struct, LEN_SINGLE_GRAPH + 30);
	Append_CRC16_Check_Sum(txbuff,length);
	HAL_UART_Transmit(&UART,txbuff,length,0xffff);	
	memset(txbuff,0,sizeof(txbuff));
	
}

void draw_seven_line(uint8_t (*txbuff)[200], uint8_t i, UART_HandleTypeDef UART, uint32_t (*Data)[12])
{
    uint32_t length = 0;
//		memset(txbuff + i, 0, 200);
    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_seven.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_seven.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_seven_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    for(int j = 0; j < 7; j++)
    {
        show_data.operate_data.show_seven.grapic_data_struct[j].graphic_name[0] = '0';
        show_data.operate_data.show_seven.grapic_data_struct[j].graphic_name[1] = '0';
        show_data.operate_data.show_seven.grapic_data_struct[j].graphic_name[2] = j;
        show_data.operate_data.show_seven.grapic_data_struct[j].operate_type = *(Data[j]+0);//Operate_type;//图形操讈E
        show_data.operate_data.show_seven.grapic_data_struct[j].graphic_type = *(Data[j]+1);//;//图形类型
        show_data.operate_data.show_seven.grapic_data_struct[j].layer = *(Data[j]+2);//5;//图瞾E
        show_data.operate_data.show_seven.grapic_data_struct[j].color = *(Data[j]+3);//1;//颜色
        show_data.operate_data.show_seven.grapic_data_struct[j].start_angle = *(Data[j]+4);//10;//
        show_data.operate_data.show_seven.grapic_data_struct[j].end_angle = *(Data[j]+5);//10;//
        show_data.operate_data.show_seven.grapic_data_struct[j].width = *(Data[j]+6);//10;//线条窥胰
        show_data.operate_data.show_seven.grapic_data_struct[j].start_x = *(Data[j]+7);//x;
        show_data.operate_data.show_seven.grapic_data_struct[j].start_y =	*(Data[j]+8);//y;
        show_data.operate_data.show_seven.grapic_data_struct[j].radius = *(Data[j]+9);//10;//皝E?
        show_data.operate_data.show_seven.grapic_data_struct[j].end_x = *(Data[j]+10);//10;//皝E?
        show_data.operate_data.show_seven.grapic_data_struct[j].end_y = *(Data[j]+11);//10;//皝E?
    }

    //show_single
    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_seven.grapic_data_struct, LEN_SEVEN_GRAPH);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
		while(HAL_UART_GetState(&UART) == HAL_UART_STATE_BUSY_TX)
		{
			osDelay(1);
		}
    HAL_UART_Transmit(&UART, *(txbuff + i), length,20);
//		HAL_UART_Transmit_DMA(&UART, *(txbuff + i), length);
		
		memset(txbuff + i, 0, 200);
    
}

void draw_five_line(uint8_t (*txbuff)[200], uint8_t i, UART_HandleTypeDef UART, uint32_t (*Data)[12])
{
    uint32_t length = 0;
//		memset(txbuff + i, 0, 200);
    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_five.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_five.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_five_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    for(int j = 0; j < 5; j++)
    {
        show_data.operate_data.show_five.grapic_data_struct[j].graphic_name[0] = '0';
        show_data.operate_data.show_five.grapic_data_struct[j].graphic_name[1] = '0';
        show_data.operate_data.show_five.grapic_data_struct[j].graphic_name[2] = j;
        show_data.operate_data.show_five.grapic_data_struct[j].operate_type = *(Data[j]+0);//Operate_type;//图形操讈E
        show_data.operate_data.show_five.grapic_data_struct[j].graphic_type = *(Data[j]+1);//;//图形类型
        show_data.operate_data.show_five.grapic_data_struct[j].layer = *(Data[j]+2);//5;//图瞾E
        show_data.operate_data.show_five.grapic_data_struct[j].color = *(Data[j]+3);//1;//颜色
        show_data.operate_data.show_five.grapic_data_struct[j].start_angle = *(Data[j]+4);//10;//
        show_data.operate_data.show_five.grapic_data_struct[j].end_angle = *(Data[j]+5);//10;//
        show_data.operate_data.show_five.grapic_data_struct[j].width = *(Data[j]+6);//10;//线条窥胰
        show_data.operate_data.show_five.grapic_data_struct[j].start_x = *(Data[j]+7);//x;
        show_data.operate_data.show_five.grapic_data_struct[j].start_y =	*(Data[j]+8);//y;
        show_data.operate_data.show_five.grapic_data_struct[j].radius = *(Data[j]+9);//10;//皝E?
        show_data.operate_data.show_five.grapic_data_struct[j].end_x = *(Data[j]+10);//10;//皝E?
        show_data.operate_data.show_five.grapic_data_struct[j].end_y = *(Data[j]+11);//10;//皝E?
    }

    //show_single
    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_five.grapic_data_struct, LEN_FIVE_GRAPH);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
		while(HAL_UART_GetState(&UART) == HAL_UART_STATE_BUSY_TX)
		{
			osDelay(1);
		}
    HAL_UART_Transmit(&UART, *(txbuff + i), length,20);
//		HAL_UART_Transmit_DMA(&UART, *(txbuff + i), length);
		
		memset(txbuff + i, 0, 200);
    
}

void draw_double_line(uint8_t (*txbuff)[200], uint8_t i, UART_HandleTypeDef UART, uint32_t (*Data)[12])
{
    uint32_t length = 0;
//		memset(txbuff + i, 0, 200);
    //按要求帖齑帧头
    length = sizeof(show_data.frame_header) + sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_double.grapic_data_struct) + sizeof(show_data.cmd_id) + sizeof(show_data.frame_tail);
    show_data.frame_header.SOF = Judge_Data_SOF;
    show_data.frame_header.Data_Length = sizeof(show_data.student_interactive_header) + sizeof(show_data.operate_data.show_double.grapic_data_struct);
    show_data.frame_header.Seq = 0;
    memcpy(txbuff + i, &show_data.frame_header, LEN_HEADER);
    Append_CRC8_Check_Sum(*(txbuff + i), LEN_HEADER);
    //发送时低八位在前
    show_data.cmd_id = 0x0301;
    memcpy(txbuff[i] + CMD_ID, &show_data.cmd_id, LEN_CMDID);
    show_data.student_interactive_header.data_cmd_id = client_custom_graphic_double_id;
    show_data.student_interactive_header.sender_ID = Judge_Self_ID;
    show_data.student_interactive_header.receiver_ID = choose_client(Judge_Self_ID);
    memcpy(txbuff[i] + STU_HEADER, &show_data.student_interactive_header, LEN_STU_HEAD);
    for(int j = 0; j < 2; j++)
    {
        show_data.operate_data.show_double.grapic_data_struct[j].graphic_name[0] = '0';
        show_data.operate_data.show_double.grapic_data_struct[j].graphic_name[1] = '0';
        show_data.operate_data.show_double.grapic_data_struct[j].graphic_name[2] = j;
        show_data.operate_data.show_double.grapic_data_struct[j].operate_type = *(Data[j]+0);//Operate_type;//图形操讈E
        show_data.operate_data.show_double.grapic_data_struct[j].graphic_type = *(Data[j]+1);//;//图形类型
        show_data.operate_data.show_double.grapic_data_struct[j].layer = *(Data[j]+2);//5;//图瞾E
        show_data.operate_data.show_double.grapic_data_struct[j].color = *(Data[j]+3);//1;//颜色
        show_data.operate_data.show_double.grapic_data_struct[j].start_angle = *(Data[j]+4);//10;//
        show_data.operate_data.show_double.grapic_data_struct[j].end_angle = *(Data[j]+5);//10;//
        show_data.operate_data.show_double.grapic_data_struct[j].width = *(Data[j]+6);//10;//线条窥胰
        show_data.operate_data.show_double.grapic_data_struct[j].start_x = *(Data[j]+7);//x;
        show_data.operate_data.show_double.grapic_data_struct[j].start_y =	*(Data[j]+8);//y;
        show_data.operate_data.show_double.grapic_data_struct[j].radius = *(Data[j]+9);//10;//皝E?
        show_data.operate_data.show_double.grapic_data_struct[j].end_x = *(Data[j]+10);//10;//皝E?
        show_data.operate_data.show_double.grapic_data_struct[j].end_y = *(Data[j]+11);//10;//皝E?
    }

    //show_single
    memcpy(txbuff[i] + STU_DATA, &show_data.operate_data.show_double.grapic_data_struct, LEN_DOUBLE_GRAPH);
    Append_CRC16_Check_Sum(*(txbuff + i), length);
		while(HAL_UART_GetState(&UART) == HAL_UART_STATE_BUSY_TX)
		{
			osDelay(1);
		}
    HAL_UART_Transmit(&UART, *(txbuff + i), length,20);
//		HAL_UART_Transmit_DMA(&UART, *(txbuff + i), length);
		
		memset(txbuff + i, 0, 200);
    
}

//operate_type graphic_type layer color start_angle end_angle width start_x start_y radius end_x end_y
uint32_t Change_Data[7][12]= {{add,oval,7,yellow,0,0,4,960,540,10,10,25},
                              {add,oval,8,yellow,0,0,4,960,200,10,10,25},
                              {add,circle,9,black,0,0,4,960,700,10,0,0}
};
//void Line_of_sight(UART_HandleTypeDef UART)
//{
//    uint32_t Data[7][12]= {{add,straight_line,0,white,0,0,4,960,520,0,960,300},
//        {add,straight_line,1,white,0,0,4,760,500,0,1160,500},
//        {add,straight_line,2,white,0,0,4,860,460,0,1060,460},
//        {add,straight_line,3,yellow,0,0,4,860,420,0,1060,420},
//        {add,straight_line,4,yellow,0,0,4,860,380,0,1060,380},
//        {add,straight_line,5,yellow,0,0,4,860,340,0,1060,340},
//        {add,straight_line,6,yellow,0,0,4,760,300,0,1160,300}
//    };
//    draw_seven_line(Tx_buff_seven,0,UART,Data);
//    draw_seven_line(Tx_buff_seven,1,huart1,Change_Data);
//}

uint8_t common_state[30]={"Player:BGC  SuperCap:000% "};
uint8_t unusedkey_state[30] = {" have not enable key, press  v"};
uint8_t unopenleg_state[30] = {" have not enable leg, open leg"};
uint8_t unbalance_state[30] = {" have not enable balance, open"};
uint8_t uncommon_state[30] =  {" dowm by over, press ctrl + z "};
uint8_t robot_state[30];
uint8_t state_coller;

uint32_t pose[7][12]  = {
	{add,straight_line,0,pink,0,0,3,100,850,0,200,850},//水平线
	{add,straight_line,0,pink,0,0,3,100,850,0,40,780},//左边上连杆
	{add,straight_line,0,pink,0,0,3,200,850,0,260,780},//右边上连杆
	{add,straight_line,0,pink,0,0,3,40,780,0,150,680},//左边下连杆
	{add,straight_line,0,pink,0,0,3,260,780,0,150,680},//右边下连杆
	{add,straight_line,0,green,0,0,3,1600,780,0,1600,830},//朝前的线
	{add,arc,0,green,30,330,3,1600,780,5,30,30},//大圆弧
    };

uint32_t line[5][12]  = {
{add,straight_line,1,yellow,0,0,17,1590,500,0,1590,620},//速度条
{add,straight_line,1,orange,0,0,3,1580,628,0,1600,628},//最高速度线
{add,rectangle,1,white,0,0,3,1580,500,0,1600,700},//速度框
{add,arc,1,zihong,330,30,3,1600,780,0,30,30},//底盘前面的圆弧
{add,rectangle,1,yellow,0,0,2,600,120,0,1400,160},//状态框
	};

uint32_t ring[3][12] = {
{add,circle,2,black,0,0,5,950,90,15,15,15},	//第一个圈
{add,circle,2,black,0,0,5,1000,90,15,15,15},//第二个圈
{add,circle,2,black,0,0,5,1050,90,15,15,15},//第三个圈
	};

	
uint32_t tmp_seven[7][12] = {
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 空
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 空
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 待定
//	{add,rectangle,1,yellow,0,0,2,600,120,0,1400,160},//状态框
	{add,straight_line,1,orange,0,0,3,1580,628,0,1600,628}, //最高速度线
//{add,circle,2,black,0,0,5,1050,90,15,15,15},//第三个圈
	{add,straight_line,1,yellow,0,0,17,1590,500,0,1590,0},//速度条
	{add,straight_line,0,green,0,0,3,1600,780,0,1600,830},//朝前的线
	{add,rectangle,1,white,0,0,3,1580,500,0,1600,700}//速度框
	};
	
uint32_t tmp_five[5][12] = {
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 空
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 空
	{add,straight_line,0,pink,0,0,0,0,0,0,0,0},// 待定
	{add,arc,1,zihong,330,30,3,1600,780,0,30,30},//底盘前面的圆弧
	{add,arc,0,green,30,330,3,1600,780,5,30,30}//大圆弧
};
/**
在直角坐标系下，一个点绕另一点顺时针旋转一定角度
* @x0：固定点x坐标
* @y0：固定点y坐标
* @x1：移动点x坐标
* @y1：移动点y坐标
* @pitch_sin：旋转角度的正弦值
* @pitch_cos：旋转角度的余弦值
*/
void Rotate_Point(float x0, float y0, float *x1, float *y1, float pitch_sin, float pitch_cos)
{
	float x2,y2;
	x2 = (*x1 - x0) * pitch_cos + (*y1 - y0) * pitch_sin + x0;
  y2 = -(*x1 - x0) * pitch_sin + (*y1 - y0) * pitch_cos + y0;
	*x1 = x2;
	*y1 = y2;
}

/**
计算姿态的每一个点坐标
* @l1：下连杆长度
* @l2：上连杆长度
* @l3：水平杆长度
* @theta1：关节电机读到的角度，竖直向下为0度，往外偏差为正
* @theta2：关节电机读到的角度，竖直向下为0度，往外偏差为正
* @pitch：水平线偏角

* @remark：因为整套程序是以下顶点为固定点的，所以想要移动整个图形的位置，只需要改变error_x、error_y里面的减去的常数
*/
void pose_cal(float l1, float l2, float l3, float theta1, float theta2, float pitch)
{
	float x[5];
	float y[5];
	float pitch_s,pitch_c;
	float theta3,theta4,theta5;
	pitch_s = arm_sin_f32(pitch);
	pitch_c = arm_cos_f32(pitch);
	x[3] = 200;
	y[3] = 850;
	x[4] = x[3] + l3;
	y[4] = y[3];
	x[1] = x[3] - l2 * arm_sin_f32(theta1);
	y[1] = y[3] - l2 * arm_cos_f32(theta1);
	x[2] = x[4] + l2 * arm_sin_f32(theta2);
	y[2] = y[4] - l2 * arm_cos_f32(theta2);
	float l4 = sqrtf(powf(y[2] - y[1], 2) + powf(x[2] - x[1], 2));
	theta3 = acosf(l4 * l4 / (2 * l4 * l1));
	theta4 = atanf(-(y[2] - y[1]) / (x[2] - x[1]));
	theta5 = theta3 + theta4;
	x[0] = x[1] + l1 * arm_cos_f32(theta5);
	y[0] = y[1] - l1 * arm_sin_f32(theta5);
	
	Rotate_Point(x[0], y[0], &x[1], &y[1], pitch_s, pitch_c);
	Rotate_Point(x[0], y[0], &x[2], &y[2], pitch_s, pitch_c);
	Rotate_Point(x[0], y[0], &x[3], &y[3], pitch_s, pitch_c);
	Rotate_Point(x[0], y[0], &x[4], &y[4], pitch_s, pitch_c);
	
	float error_x = x[0] - 160;
	float error_y = y[0] - 680;
	x[0] -= error_x;
	x[1] -= error_x;
	x[2] -= error_x;
	x[3] -= error_x;
	x[4] -= error_x;
	y[0] -= error_y;
	y[1] -= error_y;
	y[2] -= error_y;
	y[3] -= error_y;
	y[4] -= error_y;
	
	pose[0][7] = x[3];
	pose[0][8] = y[3];
	pose[0][10] = x[4];
	pose[0][11] = y[4];
	pose[1][7] = x[3];
	pose[1][8] = y[3];
	pose[1][10] = x[1];
	pose[1][11] = y[1];
	pose[2][7] = x[4];
	pose[2][8] = y[4];
	pose[2][10] = x[2];
	pose[2][11] = y[2];
	pose[3][7] = x[1];
	pose[3][8] = y[1];
	pose[3][10] = x[0];
	pose[3][11] = y[0];
	pose[4][7] = x[2];
	pose[4][8] = y[2];
	pose[4][10] = x[0];
	pose[4][11] = y[0];
}

/**
计算云台与底盘差角的UI图形起始角度与结束角度
*/
void yaw_error_cal()
{
	float start_angle,end_angle;
	start_angle = 30 + Gimble_MSG.YAW_error_angle;
	if(start_angle < 0)
		start_angle += 360;
	end_angle = 330 + Gimble_MSG.YAW_error_angle;
	if(end_angle > 360)
		end_angle -= 360;
	tmp_five[4][4] = (uint32_t)start_angle;
	tmp_five[4][5] = (uint32_t)end_angle;
	tmp_five[3][4] = (uint32_t)end_angle;
	tmp_five[3][5] = (uint32_t)start_angle;
	if(Gimble_MSG.robot_mode == VERTICAL_MODE)	//侧身的话大圆弧变橙色
	{
		tmp_five[4][3] = orange;
	}
	else
	{
		tmp_five[4][3] = green;
	}
}

/**
计算速度进度条
*/
void speed_cal(void)
{
//	uint16_t speed_now_128,speed_max_128;
//	speed_now_128 = (uint16_t)(fabs(Chass_Move_MSG.ALL_Vx_fdb) * 53.625f * 1.707f);
//	speed_max_128 = Gimble_MSG.speed_gear * 1.707f;
//	if(speed_max_128 > 127)
//		speed_max_128 = 127;
//	line[0][11] = 500 + (Gimble_MSG.speed_gear);
//	line[1][8] 	= 500 + (uint32_t)(speed_max_128 * 1.5625f);
//	line[1][11] = 500 + (uint32_t)(speed_max_128 * 1.5625f);
	tmp_seven[3][11] = 500 + (Gimble_MSG.speed_gear);
	tmp_seven[3][8] = 500 + (Gimble_MSG.speed_gear);
	tmp_seven[4][11] = 500 + (uint8_t)(Gimble_MSG.Speed_Vx);
}

void shoot_mode_cal(void)
{
	if(Gimble_MSG.shoot_mode == 1)
	{
		pose[5][3] = orange;
	}
	else
	{
		pose[5][3] = green;
	}
}

void robot_state_cal(void)
{
//	if(Gimble_MSG.Control_Mode2 == 1)	//未开启轮腿
//	{
//		memcpy(robot_state,unopenleg_state,30);
//		state_coller = qing;
//		line[4][3] = qing;
//		return;
//	}
//	else if(Gimble_MSG.Control_Mode1 == 1)	//未开启平衡
//	{
//		memcpy(robot_state,unbalance_state,30);
//		state_coller = qing;
//		line[4][3] = qing;
//		return;
//	}
//	else if(Gimble_MSG.use_mousekey == 0)	//未开启键盘模式
//	{
//		memcpy(robot_state,unusedkey_state,30);
//		state_coller = qing;
//		line[4][3] = qing;
//		return;
//	}
	if(Gimble_MSG.use_mousekey == 0)	//未开启键盘模式
	{
		memcpy(robot_state,unusedkey_state,30);
		state_coller = qing;
		line[4][3] = qing;
		return;
	}
//	else if(Chass_Move_MSG.Robot_state != 1)	//机器人不是正常状态
//	{
//		memcpy(robot_state,uncommon_state,30);
//		state_coller = qing;
//		line[4][3] = qing;
//		if(Chass_Move_MSG.Robot_state == 2)
//			memcpy(&robot_state[9], "gear", 4);
//		if(Chass_Move_MSG.Robot_state == 3)
//			memcpy(&robot_state[9], "dump", 4);
//		if(Chass_Move_MSG.Robot_state == 4)
//			memcpy(&robot_state[9], "over", 4);
//		if(Chass_Move_MSG.Robot_state == 5)
//			memcpy(&robot_state[9], "dead", 4);
//		return;
//	}
	else
	{
		memcpy(robot_state,common_state,30);
		if(super_cap.error == 1)
		{
			robot_state[21] = 'e';
			robot_state[22] = 'r';
			robot_state[23] = 'r';
			robot_state[24] = '!';
		}
		else
		{
			if(super_cap.cap_remain_energy.value / 100 > 0)
				robot_state[21] = '1';
			else
				robot_state[21] = '0';
			robot_state[22] = (int)(super_cap.cap_remain_energy.value / 10) % 10 + 48;
			robot_state[23] = (int)super_cap.cap_remain_energy.value % 10;
		}
		
		if(Gimble_MSG.Use_Super_Cap == 1)
		{
			state_coller = orange;
			line[4][3] = orange;
		}
		else
		{
			state_coller = yellow;
			line[4][3] = yellow;
		}
		return;
	}
}

void ring_cal(void)
{
	if(Gimble_MSG.warehouse == 1)
	{
		ring[1][3] = green;
	}
	else
	{
		ring[1][3] = black;
	}
	if(Gimble_MSG.robot_mode == SPINNING)	//小陀螺模式
	{
		ring[0][3] = green;
	}
	else if(Gimble_MSG.robot_mode == VERTICAL_MODE)	//侧身模式
	{
		ring[0][3] = orange;
	}
	else if(Gimble_MSG.robot_mode == FOLLOW_MODE)
	{
		ring[0][3] = qing;
	}
	else if(Gimble_MSG.robot_mode == LOSE_BALANCE){
		ring[0][3] = white;
	}
	else
	{
		ring[0][3] = black;
	}
//	if(Gimble_MSG.robot_mode == LOSE_BALANCE){
//		ring[2][3] = orange;
//	}
//	else {
//		ring[2][3] = black;
//	}
}

#define    TIME_STAMP_20MS      20
//float t1 = 1.3f;
//float t2 = 1.3f;
uint8_t	tuxin_states;

uint8_t count_for_change = 0;
uint16_t count_for_add = 0;
void tuxin_task(void const *pvParameters)
{
	while(1)
	{
		//机器人状态更新
		if(count_for_add % 451 == 0){
			show_str(robot_state,30,3,610,155,add,huart1,2,state_coller,25);
		}
		if(count_for_add % 471 == 0){
			ring[0][0] = add;
			ring[1][0] = add;
			draw_double_line(Tx_buff_Line,2,huart1,ring);
		}
		if(count_for_add % 391 == 0){
			tmp_seven[0][0] = add;
			tmp_seven[1][0] = add;
			tmp_seven[2][0] = add;
			tmp_seven[3][0] = add;
			tmp_seven[4][0] = add;
			tmp_seven[5][0] = add;
			tmp_seven[6][0] = add;
			draw_seven_line(Tx_buff_Line,0,huart1,tmp_seven);
		}
		if(count_for_add % 331 == 0){
			tmp_five[0][0] = add;
			tmp_five[1][0] = add;
			tmp_five[2][0] = add;
			tmp_five[3][0] = add;
			tmp_five[4][0] = add;
			draw_five_line(Tx_buff_Line,1,huart1,tmp_five);
		}
		if(count_for_change % 67 == 0){
			speed_cal();
			tmp_seven[0][0] = modify;
			tmp_seven[1][0] = modify;
			tmp_seven[2][0] = modify;
			tmp_seven[3][0] = modify;
			tmp_seven[4][0] = modify;
			tmp_seven[5][0] = modify;
			tmp_seven[6][0] = modify;
			draw_seven_line(Tx_buff_Line,2,huart1,tmp_seven);
		}
		if(count_for_change % 26 == 0){
			yaw_error_cal();
			tmp_five[0][0] = modify;
			tmp_five[1][0] = modify;
			tmp_five[2][0] = modify;
			tmp_five[3][0] = modify;
			tmp_five[4][0] = modify;
			draw_five_line(Tx_buff_Line,3,huart1,tmp_five);
		}
		if(count_for_change % 199 == 0)
		{
			robot_state_cal();
			show_str(robot_state,30,3,610,155,modify,huart1,2,state_coller,25);
		}

		if(count_for_change % 14 == 0)
		{
			ring_cal();
			ring[0][0] = modify;
			ring[1][0] = modify;
			draw_double_line(Tx_buff_Line,0,huart1,ring);
		}
//		show_str(robot_mode_str,30,4,1500,680,add,huart1);
//		show_str(robot_pitch_str,30,5,1500,710,add,huart1);
//		pose[0][0] = add;
//		pose[1][0] = add;
//		pose[2][0] = add;
//		pose[3][0] = add;
//		pose[4][0] = add;
//		pose[5][0] = add;
//		pose[6][0] = add;
//		draw_seven_line(Tx_buff_Line,0,huart1,pose);//车体姿态
//		shoot_mode_cal();
//		pose_cal(147,70,52, Leg_MODE.math.L_ther1 - 1.5707963f,Leg_MODE.math.L_ther2 - 1.5707963f, -Chass_Move_MSG.IMU_Chass_MSG.ol_angle[1]);
//		yaw_error_cal();
//		pose[0][0] = modify;
//		pose[1][0] = modify;
//		pose[2][0] = modify;
//		pose[3][0] = modify;
//		pose[4][0] = modify;
//		pose[5][0] = modify;
//		pose[6][0] = modify;
//		draw_seven_line(Tx_buff_Line,0,huart1,pose);//车体姿态
//		if(count_for_ui % 8 == 0){
//			line[0][0] = add;
//			line[1][0] = add;
//			line[2][0] = add;
//			line[3][0] = add;
//			line[4][0] = add;
//			draw_five_line(Tx_buff_Line,1,huart1,line);//速度框
//			speed_cal();
//			line[0][0] = modify;
//			line[1][0] = modify;
//			line[2][0] = modify;
//			line[3][0] = modify;
//			line[4][0] = modify;
//			draw_five_line(Tx_buff_Line,1,huart1,line);//速度框
//		}
//		switch(Gimble_MSG.speed_gear)
//		{
//			case 0:
//			{
//				speed_gear_str[11] = 'l';
//				speed_gear_str[12] = 'o';
//				speed_gear_str[13] = 'w';
//				speed_gear_str[14] = '\0';
//				break;
//			}
//			case 1:
//			{
//				speed_gear_str[11] = 'm';
//				speed_gear_str[12] = 'i';
//				speed_gear_str[13] = 'd';
//				speed_gear_str[14] = '\0';
//				break;
//			}
//			case 2:
//			{
//				speed_gear_str[11] = 'h';
//				speed_gear_str[12] = 'i';
//				speed_gear_str[13] = 'g';
//				speed_gear_str[14] = 'h';
//				break;
//			}
//			
//		}
//		show_str(speed_gear_str,16,3,1500,650,modify,huart1);
//		switch(Gimble_MSG.robot_mode)
//		{
//			case 0:
//			{
//				robot_mode_str[11] = 'd';
//				robot_mode_str[12] = 'o';
//				robot_mode_str[13] = 'w';
//				robot_mode_str[14] = 'n';
//				robot_mode_str[15] = '\0';
//				robot_mode_str[16] = '\0';
//				break;
//			}
//			case 1:
//			{
//				robot_mode_str[11] = 'f';
//				robot_mode_str[12] = 'o';
//				robot_mode_str[13] = 'l';
//				robot_mode_str[14] = 'l';
//				robot_mode_str[15] = 'o';
//				robot_mode_str[16] = 'w';
//				break;
//			}
//			case 2:
//			{
//				robot_mode_str[11] = 'l';
//				robot_mode_str[12] = 'o';
//				robot_mode_str[13] = 'c';
//				robot_mode_str[14] = 'k';
//				robot_mode_str[15] = '\0';
//				robot_mode_str[16] = '\0';
//				break;
//			}
//			case 3:
//			{
//				robot_mode_str[11] = 'a';
//				robot_mode_str[12] = 'i';
//				robot_mode_str[13] = 'm';
//				robot_mode_str[14] = '\0';
//				robot_mode_str[15] = '\0';
//				robot_mode_str[16] = '\0';
//				break;
//			}
//			case 4:
//			{
//				robot_mode_str[11] = 'o';
//				robot_mode_str[12] = 'n';
//				robot_mode_str[13] = 's';
//				robot_mode_str[14] = 'i';
//				robot_mode_str[15] = 'd';
//				robot_mode_str[16] = 'e';
//				break;
//			}
//			case 5:
//			{
//				robot_mode_str[11] = 'c';
//				robot_mode_str[12] = 'i';
//				robot_mode_str[13] = 'r';
//				robot_mode_str[14] = 'c';
//				robot_mode_str[15] = 'l';
//				robot_mode_str[16] = 'e';
//				break;
//			}
//			
//			
//		}
//		show_str(robot_mode_str,30,4,1500,680,modify,huart1);
//		
//		if(Gimble_MSG.target_pitch < 0)
//		{
//			robot_pitch_str[6] = '-';
//			Gimble_MSG.target_pitch *= -1;
//		}
//		else
//			robot_pitch_str[6] = '\0';
//		robot_pitch_str[7] = Gimble_MSG.target_pitch / 10000 + 48;
//		robot_pitch_str[8] = '.';
//		robot_pitch_str[9] = (Gimble_MSG.target_pitch / 1000) % 10 + 48;
//		robot_pitch_str[10] = (Gimble_MSG.target_pitch / 100) % 10 + 48;
//		robot_pitch_str[11] = (Gimble_MSG.target_pitch / 10) % 10 + 48;
//		show_str(robot_pitch_str,16,5,1500,710,modify,huart1);
		count_for_add++;
		count_for_change++;
		vTaskDelay(TIME_STAMP_20MS); //按发送协议的要求，速率为5hz以下
	}
}

