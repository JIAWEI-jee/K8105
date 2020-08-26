#include "flash.h"
#include "uart.h"
//#include "timer.h"

flash_info_t flash_info;
u8 flash_check[4] = {0x20,0x20,0x05,0x01};
u8 device_state = OFF;
u16 Input_Voltage_std = 0;

u8 get_device_state(void)
{
	return device_state;
}

void set_device_state(u8 state)
{
	device_state = state;
}


/**
  * @˵��  	����������Լ����5ms��ʱ��
  * @����  	fui_Address ���������������ڵ�����һ����ַ
  * @����ֵ ��
  * @ע		ֻҪ�����������������һ����ַ���Ϳ��Բ���������
  */
#pragma disable						//ȷ������ʱ������жϵ��²���ʧ��
void Flash_EraseBlock(u16 addr)
{
	IAP_CMD = 0xF00F;				//Flash����
	IAP_ADDR = addr;				//д�������ַ
	IAP_CMD = 0xD22D;				//ѡ�������ʽ�� ��������
	IAP_CMD = 0xE11E; 				//������IAP_ADDRL&IAP_ADDRHָ��0xFF��ͬʱ�Զ�����
}

/**
  * @˵��  	д��һ���ֽ����ݵ�Flash����
  * @����  	fui_Address ��FLASH��ַ
  *	@����	fucp_SaveData��д�������
  * @����ֵ ��
  * @ע		д֮ǰ�����ȶԲ������������в���
  */
#pragma disable						//ȷ��д��ʱ������жϵ���д��ʧ��
void FLASH_WriteData(u8 fuc_SaveData, u16 addr)
{
	IAP_DATA = fuc_SaveData;
	IAP_CMD=0xF00F;					//Flash����
	IAP_ADDR = addr;
	IAP_CMD=0xB44B;					//�ֽڱ��
	IAP_CMD=0xE11E;					//����һ�β���
}

/**
  * @˵��  	д�����ⳤ�ȵ����ݵ�FLASH����
  * @����  	fui_Address ��FLASH��ʼ��ַ
  *	@����	len �� д�����ݳ���
  *			ȡֵ��Χ��0x00-0xFF
  *	@����	*fucp_SaveArr��д������ݴ��������׵�ַ
  * @����ֵ ��
  * @ע		д֮ǰ�����ȶԲ������������в���
  */
#pragma disable						//ȷ��д��ʱ������жϵ���д��ʧ��
void Flash_WriteArr(u16 fui_Address,u8 len,u8 *fucp_SaveArr)
{
	u8 i = 0;

  EA = 0;//�ر��ж�
	for(i = 0; i < len; i++)
	{
		FLASH_WriteData(*(fucp_SaveArr++), fui_Address++); 
	}
  EA = 1;//�����ж�
}

u8 flash_read_addr(u16 address)
{
	u8 val = 0;
	val = *((unsigned char code *)(address));
	return val;
}


/**
  * @˵��  	��FLASH�����ȡ���ⳤ�ȵ�����
  * @����  	fui_Address ��FLASH��ʼ��ַ
  *	@����	fuc_Length ����ȡ���ݳ���
  *			ȡֵ��Χ��0x00-0xFF
  *	@����	*fucp_SaveArr����ȡ���ݴ�ŵ������׵�ַ
  * @����ֵ ��
  * @ע		��
  */
void Flash_ReadArr(u16 fui_Address,u8 fuc_Length,u8 *fucp_SaveArr)
{
	while(fuc_Length--)
	*(fucp_SaveArr++)=*((unsigned char code *)(fui_Address++));//��ȡ����
}

static u8 check_flash(void)
{
  u8 i = 0,cnt = 0;

  for (i = 0; i < 4; ++i)
  {
    if(flash_read_addr(FLASH_CHECK_ADDRESS+i) == flash_check[i])
    {
      cnt ++;
    }else
      return 0;
  }
  if(cnt >= 4)
    return 1;
  return 0;
}


void flash_init(void)
{
    u8 init_flag = 0;

    // FREQ_CLK = 0x10;          //ָ����ǰϵͳʱ��Ϊ16MHz
    FREQ_CLK = 0x04;//4MHZ
    //��������
    if(check_flash() == 0)//��һ�����ݳ�ʼ��
    {
        init_flag = 1;
    }
    else
    {
        gm_printf("\r\n========================================\r\n");
        Flash_ReadArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
        gm_printf("power on flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
        gm_printf("========================================\r\n");
        if(flash_info.gap > GAP_H || flash_info.timer > TIMER_8H)//�������ݲ��Ϸ�
            init_flag = 1;
    }
    if(init_flag == 1)
    {
        gm_printf("=============first init data=========== \r\n");
        flash_info.gap    = GAP_1;    //default 1 gap 
        flash_info.timer  = TIMER_0_5H; //default 0.5H
        Flash_EraseBlock(FLASH_CHECK_ADDRESS);//������ַ FLASH_CHECK_ADDRESS ��������
        Flash_WriteArr(FLASH_CHECK_ADDRESS,4,flash_check);
        Flash_WriteArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
        Flash_ReadArr(FLASH_CHECK_ADDRESS,4,flash_check);
        gm_printf("0x%04X  0x%04X  0x%04X  0x%04X \r\n",(u16)flash_check[0],(u16)flash_check[1],(u16)flash_check[2],(u16)flash_check[3]);
        gm_printf("========================================\r\n");
    }
    gm_printf("flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
}


void flah_save_data(void)
{
  // gm_printf("========================================\r\n");
  Flash_EraseBlock(FLASH_DATA_ADDRESS);
  Flash_WriteArr(FLASH_CHECK_ADDRESS,4,flash_check);
  Flash_WriteArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
  Flash_ReadArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
  // gm_printf("read flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
  // gm_printf("========================================\r\n");
}


