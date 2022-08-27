//超声波模块HC-SR04

#include "SYSCFG.h"
#include "SH.c"
#include "Ultrasonic.c"

#define eSetAddr	0
#define eDisAddr	1

BITS_T falg;	//标志集
#define IRYJM			falg.bit0    //解码标志位
#define IRZT				falg.bit1	//电平信号标志位
#define Per10ms		falg.bit2	//10ms标志位

#define UART   RA4	//串行接收引脚

u16 Count = 0;
u16 Distance = 0;
u8 Ex_Distance;

volatile u32 IRDA;
u8 Cmd;

//**************中断处理**********************************
void interrupt ISR(void)
{
	static u8 mk1, wszz;
    
	if(T0IF) 	//100us每次
	{ 
		T0IF = 0;
		TMR0 += 256-200+2;	//注意:对TMR0重新赋值TMR0在两个周期内不变化
        
		Count++;
        mk1++;
//        if(Count >= 100)
//        {
//            Count = 0;
//			Per10ms = 1;
//		}

		if(UART)	//H
		{
			IRZT=1;
		}
		else		//L
		{
			if(IRZT)//V
			{
				if(mk1>30	||	mk1<3)	//收到同步信号
				{	
					wszz = 32;
					IRDA = 0;
				}
				else
				{
					IRDA <<= 1;
					if(mk1 >= 12)	IRDA |= 0x00000001;
					if(--wszz == 0)
					{
                        IRYJM = 1;
					}
				}	
			}
            
			IRZT = 0;	
			mk1 = 0;	
		}
	}

	INTF = 0;  
}

void Sys_Init(void) 
{ 
	OSCCON = 0B01100001;    //WDT:32KHZ;	IRCF=110=16MHZ/2/2=4MHZ;
    //Bit0=1——系统时钟为内部振荡器，Bit0=0——时钟源由FOSC<2：0>决定即编译选项时选择

    INTCON= 0;      //暂禁止所有中断
	
	//端口配置
    TRISA = 0x00;    //0-输出；1-输入；
    PORTA = 0x00;	//数据寄存器
    //WPUA = 0x00;    //0-关闭弱上拉；1-打开弱上拉
    
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

    //CMCON0 = 0B00000111;     //关闭比较器，CxIN为数字IO口	*FT60F01X没有比较器*
    
	//ADCON1 = 0B01100000;     //DIVS=0,时钟选FOSC    //ADCS[2:0]=110,分频FOSC/64
	//ADCON0 = 0B10011101;     //B7,ADFM=1,结果右对齐   
                          
    //ANSEL = 0X00;	//关闭所有模拟输入	*FT60F01X没有ADC*

    OPTION = 0x08;    //Bit4=1 WDT MODE,PS=000=1:1 WDT RATE	
    //Bit7(RAPU)=0 ENABLED PULL UP PA
    //预分频器分配给 WDT
    
    T0IF = 0;	//0 = Timer0寄存器没有溢出
}

void Data_Init(void)
{
    u8 Set;
    
	delay_ms(20);
    Set = EEPROMread(eSetAddr);
    if(Set != 0x5B);
    {
        Set = 0x5B;
        Ex_Distance = 34;
        
		EEPROMwrite(eSetAddr, Set);
        EEPROMwrite(eDisAddr, Ex_Distance);
	}
    
	Ex_Distance = EEPROMread(eDisAddr);
}

void UART_Init(void)
{
	TRISA4 = 1;	//PA1输入模式
	WPUA4 = 1;	//PA1弱上拉
}

void UART_RX(void)
{
	if(MEM(IRDA,3)!=0x12 || MEM(IRDA,2)!=0x34)		return; //客户码不对.
    
	Cmd = MEM(IRDA,1);
    
	if((Cmd + MEM(IRDA,0)) !=0xFF)		return; //编码格式不对.
	
	Ex_Distance = Cmd;
    EEPROMwrite(eDisAddr, Ex_Distance);
}

//主程序
void  main(void)
{
	delay_ms(125);
    IRYJM = 0;
 
	Sys_Init();
	ULTR_Init();
	Data_Init();
    UART_Init();
    
	T0IE = 1;     		 //開定時器0中斷
	GIE = 1;       	//總中斷
	while(!IRYJM)	{if(Count >= 65535) break;}
    T0IE = 0;     		 //关定時器0中斷
    
	UART_RX();
    Count = 0;

	//主循环
	while(1)     //程序开始进入后台执行
	{ 
		//T0IE = 1;      //開定時器0中斷
		GIE = 1;       //總中斷
		ClrWdt(); 		//清看门狗
        
		//测试——延时函数
//        TRIG ^= 1;
//        delay_ms(500);
//		ULTR_Work();
//		delay_ms(200);
		
//		//测试——定时器精度
//		if(Per10ms)
//        {
//			Per10ms = 0;
//            TRIG ^= 1;
//		}
		
//		//测试——输入检测
//		if(ECHO)	TRIG = 1;
//        else 		TRIG = 0;

        ULTR_Work();
		delay_ms(25);
    }
}
