#include "pwm.h"
u8 pwm_period = 0;

void set_pwm(u8 duty)
{
	u32 tep = (u32)duty*pwm_period/100;
	PWM3D = tep;
}

void pwm_init(u16 hz)
{
	P3M5 = 0xC0;                        //P35����Ϊ�������
	PWM3_MAP = 0x35;					//PWM3ӳ��P35��


	//16MHZ
	//���ڼ��� 	= 0xFF / (Fosc / PWM��Ƶϵ��)	 PWM3C	
	//			= 0xFF /(16000000 / 128)			
	// 			= 1/255 /125000
	// 			= 490HZ
//	pwm_period = (31250/hz)-1;			//31250 = 4000000/128
	pwm_period = 0xFF;
	PWM3P = pwm_period;					//PWM����  

	PWM3C = 0x97; 						//ʹ��PWM3���ر��жϣ����������ʱ��128��Ƶ 111
}

