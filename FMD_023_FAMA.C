//Project: FMD_022A.prj
// Device: FT61F02X
// Memory: Flash 2KX14b, EEPROM 256X8b, SRAM 128X8b
// Author: 
//Company: 
//Version:
//   Date: 2022年8月17日11:42:04
//===========================================================
//===========================================================
#include "SYSCFG.h"
#include "key.c"
#include "led.c"
#include "Hardward.c"
#include "FAMA.c"

BITS_T falg;
#define Per1ms		falg.bit0
#define Per10ms		falg.bit1
#define Per500ms	falg.bit2

#define Code1 			0x12
#define Code2 			0x34
#define CmdCode 	34
#define nCmdCode 	(0xFF - CmdCode)

u32 TxData, TxData1;

//中断入口
void interrupt ISR(void)
{
	static u8 t00, t01, t02;

	if(T0IF) 
	{ ///100us每次
		T0IF = 0;
		TMR0 += 256-200+2;	//注意:对TMR0重新赋值TMR0在两个周期内不变化
        
		if(++t00 >= 10)
        {
			t00 = 0;
            Per1ms = 1;
            if(++t01 >= 10)
            {
                t01 = 0;
                Per10ms = 1;
                
				if(++t02 >= 50)
                {
					t02 = 0;
                    Per500ms = 1;
				}
			}
		}
	}
    
	INTF = 0;  
}

void Sys_Init(void) 
{ 
	OSCCON = 0B01010001;    //WDT:32KHZ;	IRCF=101=16MHZ/4=4MHZ;
    //Bit0=1——系统时钟为内部振荡器，Bit0=0——时钟源由FOSC<2：0>决定即编译选项时选择

    INTCON= 0;      //暂禁止所有中断
	
	//端口配置
	//A
    TRISA = 0x00;    //0-输出；1-输入；
    PORTA = 0x00;	//数据寄存器
    //WPUA = 0x00;    //0-关闭弱上拉；1-打开弱上拉

	//C
    TRISC     = 0x00;    //0-输出；1-输入；
    PORTC     = 0x00;
    //WPUC      = 0x00;    //0-关闭弱上拉；1-打开弱上拉
    
    MSCKCON = 0x00;
	//MSCKCON.7——保留位:写0
    //MSCKCON.6—REG_OE:1 = 打开片内稳压器，PA4 和 PC5 输出稳压器电压；\
											  0 = PA4 和 PC5 为普通 IO
    //MSCKCON.5——T2CKSRC:0 = TIMER2 时钟源为系统时钟\
												1 = TIMER2 时钟源为内部 32MHz
    //MSCKCON.4——SLVREN:1 = 打开 LVR\
											0 = 禁止 LVR\
											当 UCFG1<1:0>不为 01 时，此位无实际意义
    
    //MSCKCON.3——保留位：写0
    //MSCKCON.2——CKMAVG:1 = 打开平均模式（自动测量并累加 4 次）\
												0 = 关闭平均模式
    //MSCKCON.1——CKCNTI:1 = 使能快时钟测量慢时钟周期\
											0 = 关闭快时钟测量慢时钟周期
    //MSCKCON.0——保留位：写0

    CMCON0 = 0B00000111;     //关闭比较器，CxIN为数字IO口
    
//    ADCON1 = 0B01100000;     //DIVS=0,时钟选FOSC    //ADCS[2:0]=110,分频FOSC/64
//    ADCON0 = 0B10011101;     //B7,ADFM=1,结果右对齐                         
    ANSEL = 0X00;	//关闭所有模拟输入

    OPTION = 0x08;    //Bit4=1 WDT MODE,PS=000=1:1 WDT RATE	
    //Bit7(RAPU)=0 ENABLED PULL UP PA
    //预分频器分配给 WDT
    
    T0IF = 0;	//0 = Timer0寄存器没有溢出
}

//主函数
int main(void)
{
	Sys_Init();
    Key_Init();
    FAMA_Init();

	//开中断
	T0IE = 1;  	//开定时器0中断
	GIE = 1;   	//开总中断
    
	TxData = ((u32)Code1)<<24 | ((u32)Code2)<<16 | ((u32)CmdCode)<<8 | (u32)nCmdCode;

	while(1)
    {
		T0IE = 1;  	//開定時器0中斷
		GIE = 1;   	//總中斷
		ClrWdt(); 		//清看门狗
        
		if(Per1ms)
        {
            Per1ms = 0;
//			if(!KEY1) LED = !LED;
//			if(!KEY2) LED = !LED;
//			if(!KEY3) LED = !LED;
//			if(!KEY4) LED = !LED;
		}
        
		if(Per10ms)
        {
			Per10ms = 0;
            
			Key_Scanf();
            Key_Handle();
		}
        
		if(Per500ms)
        {
            Per500ms = 0;
			FaMa();
		}
	}
	return 0;
}
//===========================================================
