#include "SYSCFG.h"
#ifndef __SH_COMMON_H
#define __SH_COMMON_H 

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char  u8;

#define Nop() 		asm("nop")
#define ClrWdt() 	asm("clrwdt")
#define Stop() 		asm("stop")

#define  MEM(x,y)   (*(((u8 *)(&x))+y)) //ȡ������ĳһλ,X����,��y���ֽ�?

typedef struct {
    unsigned char bit0         : 1;
    unsigned char bit1         : 1;
    unsigned char bit2         : 1;
    unsigned char bit3         : 1;
    unsigned char bit4         : 1;     
    unsigned char bit5         : 1; 
    unsigned char bit6         : 1;     
    unsigned char bit7         : 1;     
} BITS_T;

//��ʱ(n*4+8)us
void delay_4us(u16 us)
{
Nop();Nop();   	
for(;us--;){Nop();Nop();Nop();};
}

//�ӕrnms
void delay_ms(u16 ms)
{
u8 us;
for(;ms--;)	
  for(us=80;us--;)
    {Nop();Nop();
 	 	ClrWdt(); //�忴�Ź�
    };
}
 
/*----------------------------------------------------
 *	�������ƣ�EEPROMread
 *	���ܣ�   ��EEPROM����
 *	���������EEAddr ���ȡ���ݵĵ�ַ
 *	���ز�����ReEEPROMread  ��Ӧ��ַ����������
 ----------------------------------------------------*/
u8 EEPROMread(u8 EEAddr)
{
	u8 ReEEPROMread;

	EEADR = EEAddr;    
	RD = 1;
	ReEEPROMread = EEDAT;     //EEPROM�Ķ����� ReEEPROMread = EEDATA;
	return ReEEPROMread;
}

/*---------------------------------------------------- 
 *	�������ƣ�EEPROMwrite
 *	���ܣ�   д���ݵ�EEPROM
 *	���������EEAddr  ��Ҫд�����ݵĵ�ַ
 *	          Data    ��Ҫд�������
 *	���ز�������
 ----------------------u8------------------------------*/
void EEPROMwrite(u8 EEAddr,u8 Data)
{
	GIE = 0;				//д���ݱ���ر��Д�
	while(GIE); 			//�ȴ�GIEد
	EEADR = EEAddr; 	 	//EEPROM�ĵ�ַ
	EEDAT = Data;		 	//EEPROM��д����  EEDATA = Data;
	EEIF = 0;
	EECON1 |= 0x34;			//��λWREN1,WREN2,WREN3��������.
	WR = 1;					//��λWR�������
	while(WR);      		//�ȴ�EEд�����
	GIE = 1;
	delay_ms(9);
}

#endif