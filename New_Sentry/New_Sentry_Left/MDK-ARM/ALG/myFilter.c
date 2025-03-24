#include "myFilter.h"
#include "arm_math.h"

Iq_IIR  yaw_omg_LPF;
Iq_IIR  Pitch_omg_LPF;




void IIR_Init(Iq_IIR* IIR)
{
	float temp[8]={1.0f,1.0f,0.0f,1.0f,-0.726542528f,0.0f,0.136728735997f,1.0f};//1Khz 50Fc
//	float temp[8]={1.0f,1.0f,0.0f,1.0f,-0.939062506f,0.0f,0.030468747091f,1.0f};//1Khz 10Fc

	/* trans to Iq math */
  IIR->b0      = temp[0];
  IIR->b1      = temp[1];
  IIR->b2      = temp[2];
  IIR->a0      = temp[3];
  IIR->a1      = temp[4];
  IIR->a2      = temp[5];
  IIR->gain0   = temp[6];
  IIR->gain1   = temp[7];
  IIR->states0 = 0;
  IIR->states1 = 0;
}

float IIR_Calc(float in, Iq_IIR* IIR)
{
  float temp = (IIR->gain0 * in) - (IIR->a1 * IIR->states0) - (IIR->a2 * IIR->states1);
  IIR->out_Iq  = ((((IIR->b0 * temp) + (IIR->b1 * IIR->states0)) + (IIR->b2 * IIR->states1)) * IIR->gain1);
  IIR->states1 = IIR->states0;
  IIR->states0 = temp;
	
	return IIR->out_Iq;
}


