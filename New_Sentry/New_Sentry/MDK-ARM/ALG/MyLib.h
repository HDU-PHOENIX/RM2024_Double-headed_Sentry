#ifndef __MYLIB_H__
#define __MYLIB_H__

#include "main.h"
#include "arm_math.h"

#define	_PI			3.14159265f
#define _2PI		6.28318531f
#define _PI_2		1.57079633f

typedef struct SlopeDataType
{
	float nowdata;
	float targetdata;
	float d_add;
}SlopeDataType;

void DataSlope_cal(SlopeDataType* SlopeData);	//数据斜坡函数
void Saturation(float* data, float max, float min); //限幅函数
float __normalangle(float* angle, float min, float max); //角度限幅

int float_to_uint(float x, float x_min, float x_max, int bits);
float uint_to_float(int x_int, float x_min, float x_max, int bits);
float RAMP_float( float final, float now, float ramp );

#endif
