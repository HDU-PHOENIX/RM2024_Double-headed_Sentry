#include "pid.h"
#include "arm_math.h"
#include "math.h"

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

//static float VariableIntegralCoefficient(float error, float absmax, float absmin);
/*参数初始化--------------------------------------------------------------*/
void pid_param_init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout,  float I_Separation, float Dead_Zone, float gama, int angle_max, int angle_min)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    if(fabs(pid->Dead_Zone) < 1e-5)
        pid->Dead_Zone = 0;
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
    pid->I_Separation = I_Separation;
    pid->Dead_Zone = Dead_Zone;
    pid->gama = gama;
    pid->angle_max = angle_max;
    pid->angle_min = angle_min;
}

float pid_caculate(PidTypeDef *pid, const float ref, const float set)
{
  	uint8_t index;
	if (pid == NULL||isnan(set)||isnan(ref)||isnan(pid->out))
	{
		pid->out = 0;
			return 0.0f;
		
	}
	pid->error[0] = set - ref;
	if(isnan(pid->error[0]) || isinf(pid->error[0]))
	{
		pid->out = 0;
			return 0.0f;
	}
	if(pid->angle_max!=pid->angle_min)
	{
		if( pid->error[0]>(pid->angle_max+pid->angle_min)/2)
			 pid->error[0]-=(pid->angle_max+pid->angle_min);
		else if( pid->error[0]<-(pid->angle_max+pid->angle_min)/2)
			 pid->error[0]+=(pid->angle_max+pid->angle_min);
	}
    if(fabs(pid->error[0])>pid->I_Separation)//误差过大，采用积分分离
    {
    	index=0;
    }
    else
    {
    	index=1;
    }
		
	pid->set = set;
	pid->fdb = ref;

    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout = (pid->Iout + pid->Ki * pid->error[0])*index;
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout=pid->Kd*(1- pid-> gama)*(pid->Dbuf[0])+pid-> gama* pid-> lastdout;
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }

	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0];
	pid-> lastdout=pid->Dout;
    return pid->out;
}

void PID_clear(PidTypeDef *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}

/*中途更改参数设定(调试)------------------------------------------------------------*/
void pid_reset(PidTypeDef	*pid, float PID[3])
{
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
}




//static float PIDRegulator(PidTypeDef *pid, const float ref, const float set)
//{
//    float result;
//    float factor;
//    float increment;
//    float pError, dError, iError;

//    if (pid == NULL)
//    {
//        return 0.0f;
//    }
//    pid->set = set;
//    pid->fdb = ref;
//    pid->error[0] = set - ref; //得到偏差值
//    result = pid->result;
//    if(fabs(pid->error[0]) > pid->Dead_Zone)
//    {
//        pError = pid->error[0] - pid->error[1];
//        iError = (pid->error[0] + pid->error[1]) / 2.0;
//        dError = pid->error[0] - 2 * (pid->error[1]) + pid->error[2];

//        //变积分系数获取
//        factor = VariableIntegralCoefficient(pid->error[0], pid->I_Separation, 0);

//        //计算微分项增量带不完全微分
//        pid->Dout = pid->Kd * (1 - pid->gama) * dError + pid->gama * pid->Dout;

//        increment = pid->Kp * pError + pid->Ki * factor * iError + pid->Dout; //增量计算
//    }
//    else
//    {
//        if((fabs(set - 0) < pid->Dead_Zone) && (fabs(ref - 0) < pid->Dead_Zone))
//        {
//            result = 0;
//        }
//        increment = 0.0;
//    }

//    result = result + increment;

//    /*对输出限值，避免超调和积分饱和问题*/
//    LimitMax(pid->out, pid->max_out);

//    pid->error[2] = pid->error[1]; //存放偏差用于下次运算
//    pid->error[1] = pid->error[0];
//    pid->result = result;
//    pid->out = ((result - 0) / (pid->max_out - 0)) * 100.0f;
//    return pid->out;
//}

//static float PIDRegulator(PidTypeDef *pid, const float ref, const float set)
//{
//    float result;
//    float factor;
//
//    if (pid == NULL)
//    {
//        return 0.0f;
//    }
//    pid->error[0] = set - ref; //得到偏差值
//    result = pid->result;
//    if(fabs(pid->error[0]) > pid->Dead_Zone)
//    {
//        pid-> Iout += pid->error[0];
//        //变积分系数获取
//        factor = VariableIntegralCoefficient(pid->error[0], pid->I_Separation, 0);

//        //计算微分项增量带不完全微分
//        pid->Dout = pid->Kd * (1 - pid->gama) * (pid->error[0] - pid->error[1]) + pid->gama * pid-> Dout;

//        result = pid->Kp * pid->error[0] + pid->Ki * pid->Iout + pid-> Dout;
//    }
//    else
//    {
//        if((fabs(set - 0) < pid->Dead_Zone) && (fabs(ref - 0) < pid->Dead_Zone))
//        {
//            result = 0;
//        }
//    }

//    /*对输出限值，避免超调和积分饱和问题*/
//    LimitMax(pid->out, pid->max_out);

//    pid->error[2] = pid->error[1]; //存放偏差用于下次运算
//    pid->error[1] = pid->error[0];
//    pid->result = result;
//    pid->out = ((result - 0) / (pid->max_out - 0)) * 100.0;
//    return pid->out;
//}
