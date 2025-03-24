#ifndef  __myFilter_H
#define  __myFilter_H

#include "main.h"

typedef struct
{
	float states0;
	float states1;
	float b0;
	float b1;
	float b2;
	float a0;
	float a1;
	float a2;
	float gain0;
	float gain1;
	
	float out_Iq;
}Iq_IIR;



extern Iq_IIR Yaw_omg_LPF;
extern Iq_IIR Pitch_omg_LPF;
extern Iq_IIR yaw_omg_LPF;

extern void IIR_Init(Iq_IIR* IIR);
extern float IIR_Calc(float in, Iq_IIR* IIR);

#endif




