#include	"SYSCFG.h"
#ifndef __ULTR_H
#define __ULTR_H

#define TRIG   	RA2	//��������
#define ECHO   RA1	//��������

#define RISK   	RA0	//Σ���ź��������	0-��ȫ��1-Σ��

extern u16 	Count;
extern u16 	Distance;
extern u8 	Ex_Distance;

u8 Dis_Count = 0;

void ULTR_Init(void)
{
	TRISA2 = 0;	//PA2���ģʽ
    TRIG = 1;
    
	TRISA0 = 0;	//PA0���ģʽ
    RISK = 0;		//0-��ȫ��1-Σ��
    
	TRISA1 = 1;	//PA4����ģʽ
	WPUA1 = 1;	//PA4������
}

void Calculate(void)
{
	Distance = Count*34/2/10;	//��λcm
	Count = 0;

	if(Distance <= Ex_Distance)	
    {
		if(++Dis_Count > 5)	RISK = 1;
	}
    else
    {
        Dis_Count = 0;	
        RISK = 0;
	}									
}

void ULTR_Work(void)
{
    TRIG = 1;
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    TRIG = 0;
    
	while(!ECHO);			//��ECHOΪ��ʱ�ȴ�
    T0IE = 1;			    	//��������
	while(ECHO);			//��ECHOΪ1�������ȴ�
	T0IE = 0;					//�رռ���
	Calculate();				//����ߵ�ƽʱ��
}

#endif
