#include	"SYSCFG.h"
#ifndef __FAMA_H
#define __FAMA_H

#include "led.c"
#include "Hardward.c"

#define Pout    RC5

extern u32 TxData, TxData1;

void FAMA_Init(void)
{
	TRISC5 = 0;		//PC5Êä³öÄ£Ê½
    Pout = 1;
}

void FaMa(void)
{ 
	u8 i,n;
	LED = 1;    

	TxData1 = TxData;
	Pout = 1;
	delay_10us(320);		//3.2ms
	Pout = 0;
	delay_10us(46);		//0.46ms

	for(i=0; i<32; i++)
	{
		if(TxData1 & 0x80000000)
		{
			Pout=1;
			delay_10us(138);
			Pout=0;
			delay_10us(46);
		}
		else
		{
			Pout = 1;
			delay_10us(46);
			Pout = 0;
			delay_10us(138);
		}
		TxData1 <<= 1;
	}       
	Pout=1;
//	delay_10us(46);    
//	Pout=0;

	LED = 0;
}

#endif
