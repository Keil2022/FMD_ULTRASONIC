#include	"SYSCFG.h"
#ifndef __ULTR_H
#define __ULTR_H

#define TRIG   	RA2	//触发引脚
#define ECHO   RA4	//接收引脚

#define RISK   	RA0	//危险信号输出引脚	0-安全；1-危险

extern u16 	Count;
extern u16 	Distance;
extern u8 	Ex_Distance;

u8 Dis_Count = 0;

void ULTR_Init(void)
{
	TRISA2 = 0;	//PA2输出模式
    TRIG = 1;
    
	TRISA0 = 0;	//PA0输出模式
    RISK = 0;		//0-安全；1-危险
    
	TRISA4 = 1;	//PA4输入模式
	WPUA4 = 1;	//PA4弱上拉
}

void Calculate(void)
{
	Distance = Count*34/2/10;	//单位cm
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
    
	while(!ECHO);			//当ECHO为零时等待
    T0IE = 1;			    	//开启计数
	while(ECHO);			//当ECHO为1计数并等待
	T0IE = 0;					//关闭计数
	Calculate();				//计算高电平时间
}

#endif

