#include "timer.h"
#include "flash.h"
#include "uart.h"
#include "lcddisplay.h"

//----------------time---------------
u16 time_cnt = 0;
u16 time_sec = 0;
u16 cnt_heat_time = 0;
u8 over_rang_time_std = 0;
u8 Gap_protect_std = 0 ;
u8 On_stay = 0;

u8 Heat_start_std = 0;
u16 time_heat = 0;
u16 temp_time = 0 ;
u8  heat_step = 0;
void set_time_sec_val ( u16 sec )
{
	time_sec = sec;
	time_cnt = 0;
	gm_printf ( "set time second:%d\r\n",sec );
}


static void set_heat_val ( void )
{
	if ( Heat_start_std == 1 )
	{
		if ( ++time_heat > Heat_Value )
		{
			//KEY_printf ( "exchange_heat_value \r\n" );
			Heat_start_std = 0;
			time_heat = 0;

       heat_step = 0;
		}
	}
}

void set_time_sec ( void )
{
	time_cnt = 0;
	if ( flash_info.timer == TIMER_ON )
	{
		time_sec = 0;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_0_5H )
	{
		time_sec = 30*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_1H )
	{
		time_sec = 60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_2H )
	{
		time_sec = 2*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_4H )
	{
		time_sec = 4*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_8H )
	{
		time_sec = 8*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	gm_printf ( "time second:%d \r\n",time_sec );
}


void time0_init ( void )
{
	/**********************************TIM0配置初始化**************************************/
	TCON1 = 0x00;						//Tx0定时器时钟为Fosc/12
	TMOD = 0x00;						//16位重装载定时器/计数器

	//Tim0计算时间 	= (65536 - 0xFACB) * (1 / (Fosc /Timer分频系数))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//定时1ms
	//反推初值 	= 65536 - ((1/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB
//    TH0 = 0xCB;  10ms
//	TL0 = 0xEB;
	TH0 = 0xF5;
	TL0 = 0x96;							//T0定时时间2ms     0x96     10ms   0xFB
	IE |= 0x02;							//打开T0中断
	TCON |= 0x10;						//使能T0
}



//10ms
void TIMER0_Rpt ( void ) interrupt TIMER0_VECTOR
{
	if ( get_device_state() == ON ) //flash_info.timer != TIMER_ON &&
	{
		time_cnt++;
		if ( time_cnt >= 500 )
		{
			set_heat_val();

			if ( On_stay == 2 )  // stay on 模式下进行保护
			{
				cnt_heat_time++;
				if ( cnt_heat_time > overrang_time )
				{
					On_stay = 0;
					cnt_heat_time = 0;
					over_rang_time_std = 1;
					//gm_printf ( "On_stay_overrang \r\n" );
				}

			}
			if ( time_sec > 0 )
			{

				time_sec--;
				if ( Gap_protect_std == 2 )
				{

					cnt_heat_time++;


					if ( cnt_heat_time > overrang_time )
					{
						over_rang_time_std = 1;
						Gap_protect_std = 0;
						cnt_heat_time = 0;
						//gm_printf ( "protect \r\n" );
					}
				}
			}
			else
			{
				if ( flash_info.timer != TIMER_ON )
				{
					set_device_state ( OFF );
					time_sec = 0;
					cnt_heat_time = 0;
					Gap_protect_std = 0;
					On_stay = 0;
					//ht1621_all_clear(); //定时时间到
					gm_printf ( "time off \r\n" );
				}
			}
			//	gm_printf("time_sec=%d \r\n",time_sec);
			time_cnt = 0;
		}
	}
	LCD_Display();
	
}

