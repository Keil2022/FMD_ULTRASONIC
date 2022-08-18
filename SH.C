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

#define  MEM(x,y)   (*(((u8 *)(&x))+y)) //取变量的某一位,X变量,第y个字节?

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

//延时(n*4+8)us
void delay_4us(u16 us)
{
Nop();Nop();   	
for(;us--;){Nop();Nop();Nop();};
}

//延時nms
void delay_ms(u16 ms)
{
u8 us;
for(;ms--;)	
  for(us=80;us--;)
    {Nop();Nop();
 	 	ClrWdt(); //清看门狗
    };
}
 
/*----------------------------------------------------
 *	函数名称：EEPROMread
 *	功能＿   读EEPROM数据
 *	输入参数：EEAddr 需读取数据的地址
 *	返回参数；ReEEPROMread  对应地址读出的数捿
 ----------------------------------------------------*/
u8 EEPROMread(u8 EEAddr)
{
	u8 ReEEPROMread;

	EEADR = EEAddr;    
	RD = 1;
	ReEEPROMread = EEDAT;     //EEPROM的读数据 ReEEPROMread = EEDATA;
	return ReEEPROMread;
}

/*---------------------------------------------------- 
 *	函数名称：EEPROMwrite
 *	功能＿   写数据到EEPROM
 *	输入参数：EEAddr  需要写入数据的地址
 *	          Data    需要写入的数据
 *	返回参数：无
 ----------------------u8------------------------------*/
void EEPROMwrite(u8 EEAddr,u8 Data)
{
	GIE = 0;				//写数据必须关闭中斿
	while(GIE); 			//等待GIE丿
	EEADR = EEAddr; 	 	//EEPROM的地址
	EEDAT = Data;		 	//EEPROM的写数据  EEDATA = Data;
	EEIF = 0;
	EECON1 |= 0x34;			//置位WREN1,WREN2,WREN3三个变量.
	WR = 1;					//置位WR启动编程
	while(WR);      		//等待EE写入完成
	GIE = 1;
	delay_ms(9);
}

#endif
