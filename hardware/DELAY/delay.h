#ifndef __DELAY_H__
#define __DELAY_H__
#include "HC89F3541.H"

// #define MAIN_Fosc        16000000UL    //������ʱ��HZ
#define MAIN_Fosc        4000000UL    //������ʱ��HZ
void delay_us(u16 us);
void delay_ms(u16 ms);
//void lcd_delay_us(u16 time);

#endif


