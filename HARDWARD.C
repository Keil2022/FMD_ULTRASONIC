#include "SYSCFG.h"
#ifndef __HARDWARD_H
#define __HARDWARD_H 

#define Ver 5000

#define  Max8bit	255
#define  Max10bit	1023
#define  Max12bit	4095
#define  Max16bit	65535
#define  Max24bit	2^24-1
#define  Max32bit	2^32-1

#define  ADV8(x)    ((u32)(x)*Max8bit/Ver)  	//8bit AD转换倍率
#define  ADV10(x)  	((u32)(x)*Max10bit/Ver)  	//10bit AD转换倍率
#define  ADV12(x)  	((u32)(x)*Max12bit/Ver)  	//12bit AD转换倍率
#define  ADV16(x)   ((u32)(x)*Max16bit/Ver)  	//16bit AD转换倍率

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
//#define SLEEP() 	asm("SLEEP")
#define  MEM(x,y)   (*(((u8 *)(&x))+y)) //取变量的某一位,X变量,第y个字节?

//标志集
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
	for(;us--;)	{Nop();Nop();Nop();}
}

//延时(n*4+8)us
void delay_10us(u16 us)
{   	
	Nop();Nop(); 
	for(;us--;)	{Nop();Nop();Nop();Nop();}
}

//延時nms
void delay_ms(u16 ms)
{
	u8 us;
	for(;ms--;)	
		for(us=80;us--;)
		{
            Nop();Nop();
			ClrWdt(); //清看门狗
		}
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
 
           
/*-------------------------------------------------
 *  函数吿  GET_ADC_DATA
 *	功能＿ 读取通道ADC倿
 *  输入＿ AN_CN 通道序号
 *  输出＿ INT类型AD倿单次采样无滤泿
 --------------------------------------------------*/
/*u16 GET_ADC_DATA (u8 AN_CH) 
{ 
	u8 i;
	u8 	ADCON0Buff; 
	u16  tBuffer = 0;
	u16  ADC_DATA=0;
	 
	ADCON0Buff = ADCON0 & 0B11100011; 	//清空通道倿

	AN_CH <<=2;              
	ADCON0Buff |=  AN_CH;   			//(BIT4-BIT2)CHS=010,选择AN2通道      

	ADCON0 = ADCON0Buff; 				//重新加载通道倿
    delay_4us(20); 
	GO_DONE = 1;             			//启动ADC 
	while( GO_DONE==1 );    			//等待ADC转换完成

	ADC_DATA =  ADRESH;
	ADC_DATA <<=8;
	ADC_DATA |= ADRESL;      			//10Bit ADC值整吿
	tBuffer =  ADC_DATA;
	return  tBuffer;
} 

*/  
//10位AD轉換程序,参考电压为VCC
//adchs 為模擬通道
u16 ADC10(u8 adchs)	
{
	u16 adz;
	ADCON1=0x10;
	ADCON0= 0x81 | adchs;
	Nop();
	Nop();
	Nop();
	GO_DONE= 1;
	while(GO_DONE);
	MEM(adz,1)=ADRESH;
	MEM(adz,0)=ADRESL;
	return adz;
}

//8位AD轉換程序,参考电压为VCC
//adchs 為模擬通道
u8 ADC8(u8 adchs)	
{
	ADCON1 = 0x10;
	ADCON0 = 0x01 | adchs;
	Nop();
	Nop();
	Nop();
	Nop();
	GO_DONE = 1;
	while(GO_DONE);
	return (ADRESH);
}

#endif
