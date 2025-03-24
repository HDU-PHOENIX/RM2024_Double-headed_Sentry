#include "Kalman.h"

void Kalman_Init(KalmanType *p ,float Q ,float R)
{
	p->lastvalue =0.f;
	p->P1        =10.f;
	p->Q         =Q;
	p->R         =R;
	p->Kg        =0.f;
	p->P         =10.f;
}

float Kalman_Cal(KalmanType *p ,float nowvalue )
{
  float kalman_adc;
	p->P         = p->P1 +p->Q;
  p->Kg        = p->P / (p->P + p->R);
	kalman_adc   = p->lastvalue + p->Kg * (nowvalue - p->lastvalue);
	p->P1        = (1 - p->Kg) * p->P;
	p->P         = p->P1;
	p->lastvalue = kalman_adc;
	return (float)kalman_adc;
}


