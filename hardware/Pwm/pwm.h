#ifndef __PWM_H__
#define __PWM_H__
#include "HC89F3541.H"
#define heat_out P3_5
void pwm_init(void);
void set_pwm(u8 duty);
#endif

