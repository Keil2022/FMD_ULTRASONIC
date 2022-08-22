#include	"SYSCFG.h"
#ifndef __KEY_H
#define __KEY_H

#include "led.c"

#define KEY1    RA7
#define KEY2    RA6
#define KEY3    RC1
#define KEY4    RA4

#define KEY_Press 	1
#define KEY_Lift 		0
#define	True			1
#define	Fault			0

#define KEY_ShortTime		5	//50ms
#define KEY_LongTime		100	//1s
#define KEY_EndTime		300	//3s
#define Inching_SetTime	5

#define Back_Off_BrakeTime		1
#define Back_Off_BackwardTime	(Back_Off_BrakeTime+1)

bit KEY1_ShortOK;	//短按有效
bit KEY1_LongOK;		//长按有效
bit KEY1_EndOK;		//结束按键
u8 KEY1_Phase;		//按键阶段
u16 KEY1_TimeCount;	//按键时间

bit KEY2_ShortOK;
bit KEY2_LongOK;	
bit KEY2_EndOK;
u8 KEY2_Phase;
u16 KEY2_TimeCount;

bit KEY3_ShortOK;
bit KEY3_LongOK;	
bit KEY3_EndOK;
u8 KEY3_Phase;
u16 KEY3_TimeCount;

bit KEY4_ShortOK;
bit KEY4_LongOK;	
bit KEY4_EndOK;
u8 KEY4_Phase;
u16 KEY4_TimeCount;

bit MODE; 	//0-点动模式； 1-连动模式
u8 Inching_Time;
u8 Back_Off;

void Key_Init(void)
{
	TRISC |= 0x02;	//PC1输入模式
	WPUC |= 0x02;	//PC1弱上拉
   
	TRISA |= 0xD0;	//PA4 6 7输入模式
	WPUA |= 0xD0;	//PA4 6 7弱上拉 
    
	KEY1_Phase = 0;
    KEY1_TimeCount = 0;
    
	KEY2_Phase = 0;
    KEY2_TimeCount = 0;
    
	KEY3_Phase = 0;
    KEY3_TimeCount = 0;
    
	KEY4_Phase = 0;
    KEY4_TimeCount = 0;
}

void Key1_Scanf(void)
{
	switch(KEY1_Phase)
    {
		case 0:
					//按键1
					if(KEY1 == KEY_Press)
					{
						KEY1_TimeCount++;
						if(KEY1_TimeCount >=  KEY_LongTime)
						{
							//KEY1_Phase = 1;	//注释掉为 不跳转长按功能
						}
					}
					else
					{
						if(KEY1_TimeCount >= KEY_ShortTime)		KEY1_ShortOK = True;
						KEY1_TimeCount = 0;
					}
		break;
        
        
        case 1:
					if(KEY1 == KEY_Press)
					{
						KEY1_TimeCount++;
						if(KEY1_TimeCount >= KEY_EndTime)
						{
							KEY1_Phase = 2;
                            //KEY1_LongOK = True;
						}
					}
					else
					{
						KEY1_Phase = 0;
						KEY1_TimeCount = 0;
                        KEY1_LongOK = True;
					}
		break;
        
        
        case 2:
					if(KEY1 == KEY_Lift)	//等待松手
					{
						KEY1_Phase = 0;
						KEY1_TimeCount = 0;
                        KEY1_LongOK = True;
					}
		break;
        

        default:
					KEY1_Phase = 0;
					KEY1_TimeCount = 0;
		break;
	}
}

void Key_Scanf(void)
{
	//按键1
	if(KEY1 == KEY_Press)
	{
		KEY1_TimeCount++;
		if(KEY1_TimeCount >=  KEY_LongTime)
		{
			//KEY1_Phase = 1;	//注释掉为 不跳转长按功能
		}
        else {/*do nothing*/}
	}
	else
	{
		if(KEY1_TimeCount >= KEY_ShortTime)	KEY1_ShortOK = True;
        else {/*do nothing*/}

		KEY1_TimeCount = 0;
	}
	
	//按键2
	if(KEY2 == KEY_Press)
	{
		KEY2_TimeCount++;
		if(KEY2_TimeCount >=  KEY_LongTime)
		{
			//KEY2_Phase = 1;	//注释掉为 不跳转长按功能
		}
        else {/*do nothing*/}
	}
	else
	{
		if(KEY2_TimeCount >= KEY_ShortTime)	KEY2_ShortOK = True;
        else {/*do nothing*/}
		
		KEY2_TimeCount = 0;
	}
	
	//按键3
	if(KEY3 == KEY_Press)
	{
		KEY3_TimeCount++;
		if(KEY3_TimeCount >=  KEY_LongTime)
		{
			//KEY2_Phase = 1;	//注释掉为 不跳转长按功能
		}
        else {/*do nothing*/}
	}
	else
	{
		if(KEY3_TimeCount >= KEY_ShortTime)	KEY3_ShortOK = True;
        else {/*do nothing*/}
        
		KEY3_TimeCount = 0;
	}	
	
	//按键4
	if(KEY4 == KEY_Press)
	{
		KEY4_TimeCount++;
		if(KEY4_TimeCount >=  KEY_LongTime)
		{
			//KEY2_Phase = 1;	//注释掉为 不跳转长按功能
            
		}
        else {/*do nothing*/}
	}
	else
	{
		if(KEY4_TimeCount >= KEY_ShortTime)	KEY4_ShortOK = True;
        else {/*do nothing*/}

		KEY4_TimeCount = 0;
	}    
}

void Key_Handle(void)
{
	if(KEY1_ShortOK == True)
    {
		KEY1_ShortOK = Fault;
		
	}
    else {/*do nothing*/}
    
	if(KEY2_ShortOK == True)
    {
		KEY2_ShortOK = Fault;
		
	}
    else {/*do nothing*/}
    
	if(KEY3_ShortOK == True)
    {
		KEY3_ShortOK = Fault;
		
	}
    else {/*do nothing*/}
    
	if(KEY4_ShortOK == True)
    {
		KEY4_ShortOK = Fault;
        
	}
    else	 {/*do nothing*/}
}

#endif

