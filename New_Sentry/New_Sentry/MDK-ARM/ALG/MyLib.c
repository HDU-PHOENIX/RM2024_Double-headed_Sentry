#include "MyLib.h"

//斜坡函数计算
void DataSlope_cal(SlopeDataType* SlopeData)
{
	if(SlopeData->nowdata > SlopeData->targetdata)
	{
		SlopeData->nowdata -= SlopeData->d_add;
	}
	else if(SlopeData->nowdata < SlopeData->targetdata)
	{
		SlopeData->nowdata += SlopeData->d_add;
	}
}

//限幅函数
void Saturation(float* data, float max, float min)
{
	if(*data > max)	*data = max;
	if(*data < min)	*data = min;
}

//角度限幅
float __normalangle(float* angle, float min, float max)
{
	float d;
	d = max - min;
	if(*angle < min)
	{
		*angle += d;
	}
	else if(*angle > max)
	{
		*angle -= d;
	}
	return *angle;
}

/**
  * @brief  斜坡函数,使目标输出值缓慢等于期望值
  * @param  期望最终输出,当前输出,变化速度(越大越快)
  * @retval 当前输出
  */
float see_now=0;
float RAMP_float( float final, float now, float ramp ){
	see_now=now;
	float buffer = 0;
	buffer = final - now;
	if (buffer > 0){
			if (buffer > ramp){  
					now += ramp;
			}   
			else{
					now += buffer;
			}
	}
	else{
			if (buffer < -ramp){
					now += -ramp;
			}
			else{
					now += buffer;
			}
	}
	return now;
}

/************变量类型转换***********/
int float_to_uint(float x, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	return (int)((x-offset)*((float)((1<<bits)-1))/span);
}

float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}


