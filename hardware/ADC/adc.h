#ifndef __ADC_H__
#define __ADC_H__
#include "HC89F3541.H"



void adc_init(void);
u16 Detection_Input_Voltage(void);
void get_voltage ( u16* u1_voltage,u16* u3_voltage );

#endif
