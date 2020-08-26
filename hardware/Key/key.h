#ifndef __KEY_H__
#define __KEY_H__
#include "HC89F3541.H"

#define KEY_1 	P0_1
#define KEY_2 	P2_7
#define KEY_3 	P0_0

#define Insert_Test P0_2

#define KEY_1_PRES 	1	//KEY1����
#define KEY_2_PRES	2	//KEY2����
#define KEY_3_PRES	3	//KEY3����

#define KEY_STATE_0         0       // ����״̬
#define KEY_STATE_1         1
#define KEY_STATE_2         2
#define KEY_STATE_3         3
 
#define LONG_KEY_TIME       200    //����ʱ��
#define SINGLE_KEY_TIME     3      // �̰�ʱ��
 
#define N_KEY         0             // û�а�������

#define KEY_1_SHORT  4
#define KEY_1_LONG   5


void key_init(void);
u8 key_scan(void);

#endif

