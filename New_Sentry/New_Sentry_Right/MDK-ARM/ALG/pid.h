#ifndef __bsp_pid
#define __bsp_pid
#include "main.h"


typedef enum
{
    PID_POSITION = 0,//λ��ʽPID
    PID_DELTA, //����ʽPID
} PID_MODE;

typedef struct PidTypeDef
{
    float Dead_Zone; //���������ֵ
    uint8_t mode;
    //PID ������
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //������
    float max_iout; //���������

    float set; //�趨ֵ
    float fdb; //����ֵ

    float out;
    float lastout;
    float Pout;
    float Iout;
    float Dout;
    float Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    float error[3]; //����� 0���� 1��һ�� 2���ϴ�
    int angle_max;
    int angle_min;	//�Ƕ�����ֵ ����һ��Բ�ڣ�0���360�����ڣ���max=360��min=0
    //			��һ������� 0��8192���ڣ���max=8192��min=0
    float I_Separation; //���ַ�����ֵ
    float gama;			//΢�������˲�ϵ��
    float lastdout;		//��һ��΢�����


} PidTypeDef;



extern float pid_caculate(PidTypeDef *pid, const float ref, const float set);
extern void pid_param_init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout,  float I_Separation, float Dead_Zone, float gama, int angle_max, int angle_min);
void PID_clear(PidTypeDef *pid);
void pid_reset(PidTypeDef	*pid, float PID[3]);
#endif
