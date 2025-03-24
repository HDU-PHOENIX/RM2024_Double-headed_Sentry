#include "MyLib.h"

//б�º�������
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

//�޷�����
void Saturation(float* data, float max, float min)
{
	if(*data > max)	*data = max;
	if(*data < min)	*data = min;
}

//�Ƕ��޷�
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

/************��������ת��***********/
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


