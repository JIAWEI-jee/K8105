/**
*	************************************************************************************
*	@Examle Version		K8104
*	@Demo	Version		V1.00.00
*	@Date				2020.6。23
*	************************************************************************************
*
**/

#define	ALLOCATE_EXTERN
#include "HC89F3541.H"
#include "delay.h"
#include "adc.h"
#include "flash.h"
#include "key.h"
#include "uart.h"
#include "pwm.h"
#include "wdt.h"
#include "timer.h"
#include "lcddisplay.h"
#include "PID.h"

#define SKU 8105
#define SOFT_VER "1.00.00"


u16 adc_cnt = 0;
u8  first_heat_std = 0;
u8  error_std = 0;

void Clock ( void );
void Set_Temp ( void );
void Controll_Heat ( void );
//void Protect ( void );
void Detection_Input ( void );



/***************************************************/
/*
函数名称；key_handle()
函数功能：按键处理函数
入口参数：null
出口参数：null
函数说明；处理按键返回的键值调整相应功能
*/
/********************************************************/

static void key_handle ( void )
{
	u8 key_val = 0;

	if ( error_std == 0 )

	{
		key_val = key_scan();
	}
	else
	{
		key_val = 0;
	}
	if ( key_val == KEY_1_PRES )
	{
		KEY_printf ( " key_scan\r\n" );
		if ( get_device_state() == ON )
		{
			set_device_state ( OFF );
			set_pwm ( 0 );
		}
		else
		{
			set_device_state ( ON );
			set_time_sec();
			Set_Temp ( );
			//	gm_printf ( " spid.iSetVal = %d \r\n",  spid.iSetVal);
			first_heat_std = 1;

		}
	}
	else if ( get_device_state() == ON )
	{



		if ( key_val == KEY_2_PRES )
		{

			KEY_printf ( " KEY_2_PRES\r\n" );
			if ( flash_info.gap < GAP_9 )
			{
				flash_info.gap++;

			}
			else
			{
				flash_info.gap = GAP_1;
			}


			first_heat_std = 1;
			Set_Temp ( );
			//	gm_printf ( " spid.iSetVal = %d \r\n",  spid.iSetVal);
			lcd_display_gap ( flash_info.gap );
			//set_time_sec();
			flah_save_data();
		}
		else if ( key_val == KEY_3_PRES ) //??
		{

			if ( flash_info.timer < 0x05 )
			{
				flash_info.timer++;
			}
			else
			{
				flash_info.timer = TIMER_ON;
			}
			if ( flash_info.gap > GAP_1 )
			{
				if ( flash_info.timer == TIMER_ON )
				{
					On_stay = 2;
				}
				else if ( flash_info.timer > TIMER_2H )
				{
					Gap_protect_std = 2;
				}
			}
			//	KEY_printf ( "timer:%d \r\n", ( u16 ) flash_info.timer );
			lcd_display_time ( flash_info.timer );
			set_time_sec();
			flah_save_data();
		}
	}
}

/***************************************************/
/*
函数名称；temp_calc()
函数功能：温度处理函数
入口参数：uRt ,uRw
出口参数：temperature
函数说明；根据ADC侦测到的值计算实际温度值
*/
/********************************************************/

u16 temp_calc ( u16 uRt,u16 uRw )
{
	u16 i = 0;
	u16 basi_tmp = 40;

	float u1 = 0;
	float u3 = 0;
	if ( uRt < 90 )
	{
		return 0xff;
	}

	u1 = ( float ) uRt/1000;
	u3 = 	( float ) uRw /1000;
	u1 = (  u3 - u1 ) / u1 * 750;
	if ( u1 < 0 )
	{
		return 0xff;
	}
//	gm_printf ( "R = %f  \r\n",u1 );


	if ( u1 > Temperature_Value )
	{
		i = ( u1 - Temperature_Value ) /0.4;
		//gm_printf("over 40  i:%d \r\n",i);
		basi_tmp = basi_tmp + i;
	}

	else
	{
		i = ( Temperature_Value - u1 ) /0.4;
		//  gm_printf("under 40  i:%d \r\n",i);
		basi_tmp = basi_tmp - i;
	}

//	gm_printf("basi_tmp:%d \r\n",basi_tmp);
	if ( flash_info.gap == GAP_3 )
	{
		basi_tmp = basi_tmp - 5;
	}

	return  basi_tmp;
}


/*********************************************************/
/*
函数名称；temperature_handle()
函数功能：温度处理函数
入口参数：null
出口参数：null
函数说明；根据ADC侦测到的值算出温度值，通过温度的值进行温度控制
*/
/********************************************************/

void temperature_handle ( void )
{
	u16 temp = 0;
	u16 adc_val1 = 0,adc_val2 = 0;

	adc_cnt++;

	if ( adc_cnt >= 50000 )
	{
		adc_cnt = 0;
		get_voltage ( &adc_val1,&adc_val2 );

		//	KEY_printf ( "adv1 = %d adv2 =%d \r\n",adc_val1,adc_val2 );  //pjw set
		temp = temp_calc ( adc_val1, adc_val2 );
		//	KEY_printf ( "temp val:%d \r\n",temp );


		if ( adc_val1 > 90 ) //adc_val1 > 50
		{
			if ( get_device_state() == ON )
			{
				lcd_off ( ON );
				if ( first_heat_std == 1 )
				{
					first_heat_std = 0;
					if ( temp > 50 )
					{
						Heat_start_std = 0;
						heat_step = 0;
						//	KEY_printf ( "first_heat_std heat_step = 0; \r\n" );  //pjw set
					}
					else
					{
						Heat_start_std = 1;
						heat_step = 1;
						//	KEY_printf ( "first_heat_std heat_step = 1; \r\n" );  //pjw set
					}
				}

				spid.iCurVal = temp*10;
				PID_Operation ();
				lcd_display_time ( flash_info.timer );
				lcd_display_gap ( flash_info.gap );
				Controll_Heat (  );
			}
			else
			{
				lcd_off ( OFF );
				set_pwm ( 0 );
				lcd_clear_all();
			}
			error_std = 0;
		}
		else
		{

			lcd_off ( Error );
			lcd_error (  );
			error_std = 1;
		}


	}


}








void main()
{
	Clock();
	uart_init();
	adc_init();
	EA = 1;
	time0_init ();						//打开总中断
	key_init();
	flash_init();
	PID_Init();
	pwm_init ( 200 );
	wdt_init ( 2 );
	LCD_Init();
	lcd_display_On();
	delay_ms ( 1200 );
	lcd_clear_all ();
	Detection_Input();

	gm_printf ( "\r\n==================================\r\n" );
	gm_printf ( "sku:K%d \r\n", ( u16 ) SKU );
	gm_printf ( "soft version:%s \r\n",SOFT_VER );
	gm_printf ( "gap %d \r\n", ( u16 ) flash_info.gap );      //档位
	gm_printf ( "timer %d \r\n", ( u16 ) flash_info.timer );  // 定时
	gm_printf ( "Input_Voltage_std = %d\r\n",Input_Voltage_std ); //输入电压
	gm_printf ( "==================================\r\n" );

	while ( 1 )
	{


		temperature_handle();
		key_handle ();
		//	Protect ();
		clear_wdt();

	}
}



void Clock ( void )
{
	/************************************系统初始化****************************************/
	CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz
}


/***************************************************/
/*
函数名称；Controll_Heat()
函数功能：加热PWM控制
入口参数：null
出口参数：null
函数说明；根据控制算法得到的占空比输出功率
*/
/********************************************************/

void Controll_Heat ( void )
{
	//gm_printf ( " spid.iPriVal = %d \r\n",  spid.iPriVal);
	u8 heat_step_val = 100;
	if ( Input_Voltage_std == V_24_status )
	{
		spid.iPriVal = spid.iPriVal/2;
		heat_step_val = 50;
	}
	if ( heat_step == 1 )
	{
		set_pwm ( heat_step_val );
	}
	else if ( heat_step == 0 )

	{
		set_pwm ( ( u8 ) spid.iPriVal ); //(u8) spid.iPriVal
	}
}

/***************************************************/
/*
函数名称；Set_Temp()
函数功能：设定目标温度
入口参数：null
出口参数：null
函数说明；根据不同档位设定要加热到的目标温度
*/
/********************************************************/

void Set_Temp ( void )
{


	switch ( flash_info.gap )
	{
		case GAP_WARM:

			break;
		case GAP_1:
			spid.iSetVal = GAP_1_temp*10;
			break;
		case GAP_2:
			spid.iSetVal = GAP_2_temp*10;
			break;
		case GAP_3:
			spid.iSetVal = GAP_3_temp*10;
			break;
		case GAP_4:
			spid.iSetVal = GAP_4_temp*10;
			break;
		case GAP_5:
			spid.iSetVal = GAP_5_temp*10;
			break;
		case GAP_6:
			spid.iSetVal = GAP_6_temp*10;
			break;
		case GAP_7:
			spid.iSetVal = GAP_7_temp*10;
			break;
		case GAP_8:
			spid.iSetVal = GAP_8_temp*10;
			break;
		case GAP_9:
			spid.iSetVal = GAP_9_temp*10;
			break;
	}
}

/*********************************************************/
/*
函数名称；Protect()
函数功能：PID运算
入口参数：null
出口参数：null
函数说明；大于Gap1档位情况下加热超过2小时会跳到Gap1
*/
/********************************************************/
//void Protect ( void )
//{
//	if ( over_rang_time_std == 1 )
//	{
//		if ( flash_info.gap > GAP_1 )
//		{
//			flash_info.gap = GAP_1;
//			Set_Temp ( );
//			flah_save_data();
//			over_rang_time_std = 0;
//		}
//	}
//}


void Detection_Input ( void )
{
	u16 input = 0;
	input = Detection_Input_Voltage( );
	if ( input > 1600 )
	{
		//gm_printf ( "input24 = %d\r\n" ,input); //输入电压
		Input_Voltage_std = V_24_status;
	}
	else
	{
		//gm_printf ( "input12 = %d\r\n" ,input); //输入电压
		Input_Voltage_std = V_12_status;
	}
}

