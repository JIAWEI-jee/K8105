#include "wdt.h"


void wdt_init(u8 time)
{
    WDTC = 0x17;						//����WDT��λ���������/����ģʽ�����У�1024��Ƶ
    //WDTʹ�õ���RC44Kʱ�ӣ�WDT_CLOCK_1024��1024��Ƶ����ֵΪ0xFF
	//��ʱʱ�� 	= 1024 * 0xFF / 44000
	//			= 5.93s
	// WDTCCR = 0xFF;						//д��00ʱ�����ر�WDT���ܣ������ر��ڲ���ƵRC����
										//���൱�ڽ�ֹWDT��д���0����ʱ��������WDT��
	WDTCCR = (time)*(43);	//2S   2*44000/1024 = 2*43;
    // WDTCCR = 86;   //2S   2*44000/1024 = 2*43;
}

void clear_wdt(void)
{
	WDTC |= 0x10;                   //�幷									
}





