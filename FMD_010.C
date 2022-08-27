//������ģ��HC-SR04

#include "SYSCFG.h"
#include "SH.c"
#include "Ultrasonic.c"

#define eSetAddr	0
#define eDisAddr	1

BITS_T falg;	//��־��
#define IRYJM			falg.bit0    //�����־λ
#define IRZT				falg.bit1	//��ƽ�źű�־λ
#define Per10ms		falg.bit2	//10ms��־λ

#define UART   RA4	//���н�������

u16 Count = 0;
u16 Distance = 0;
u8 Ex_Distance;

volatile u32 IRDA;
u8 Cmd;

//**************�жϴ���**********************************
void interrupt ISR(void)
{
	static u8 mk1, wszz;
    
	if(T0IF) 	//100usÿ��
	{ 
		T0IF = 0;
		TMR0 += 256-200+2;	//ע��:��TMR0���¸�ֵTMR0�����������ڲ��仯
        
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
				if(mk1>30	||	mk1<3)	//�յ�ͬ���ź�
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
    //Bit0=1����ϵͳʱ��Ϊ�ڲ�������Bit0=0����ʱ��Դ��FOSC<2��0>����������ѡ��ʱѡ��

    INTCON= 0;      //�ݽ�ֹ�����ж�
	
	//�˿�����
    TRISA = 0x00;    //0-�����1-���룻
    PORTA = 0x00;	//���ݼĴ���
    //WPUA = 0x00;    //0-�ر���������1-��������
    
    MSCKCON = 0x00;
	//MSCKCON.7��������λ:д0
    //MSCKCON.6���VREG_OE:1 = ��Ƭ����ѹ����PA4 �� PC5 �����ѹ����ѹ��\
											  0 = PA4 �� PC5 Ϊ��ͨ IO
    //MSCKCON.5����T2CKSRC:0 = TIMER2 ʱ��ԴΪϵͳʱ��\
												1 = TIMER2 ʱ��ԴΪ�ڲ� 32MHz
    //MSCKCON.4����SLVREN:1 = �� LVR\
											0 = ��ֹ LVR\
											�� UCFG1<1:0>��Ϊ 01 ʱ����λ��ʵ������
    
    //MSCKCON.3��������λ��д0
    //MSCKCON.2����CKMAVG:1 = ��ƽ��ģʽ���Զ��������ۼ� 4 �Σ�\
												0 = �ر�ƽ��ģʽ
    //MSCKCON.1����CKCNTI:1 = ʹ�ܿ�ʱ�Ӳ�����ʱ������\
											0 = �رտ�ʱ�Ӳ�����ʱ������
    //MSCKCON.0��������λ��д0

    //CMCON0 = 0B00000111;     //�رձȽ�����CxINΪ����IO��	*FT60F01Xû�бȽ���*
    
	//ADCON1 = 0B01100000;     //DIVS=0,ʱ��ѡFOSC    //ADCS[2:0]=110,��ƵFOSC/64
	//ADCON0 = 0B10011101;     //B7,ADFM=1,����Ҷ���   
                          
    //ANSEL = 0X00;	//�ر�����ģ������	*FT60F01Xû��ADC*

    OPTION = 0x08;    //Bit4=1 WDT MODE,PS=000=1:1 WDT RATE	
    //Bit7(RAPU)=0 ENABLED PULL UP PA
    //Ԥ��Ƶ������� WDT
    
    T0IF = 0;	//0 = Timer0�Ĵ���û�����
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
	TRISA4 = 1;	//PA1����ģʽ
	WPUA4 = 1;	//PA1������
}

void UART_RX(void)
{
	if(MEM(IRDA,3)!=0x12 || MEM(IRDA,2)!=0x34)		return; //�ͻ��벻��.
    
	Cmd = MEM(IRDA,1);
    
	if((Cmd + MEM(IRDA,0)) !=0xFF)		return; //�����ʽ����.
	
	Ex_Distance = Cmd;
    EEPROMwrite(eDisAddr, Ex_Distance);
}

//������
void  main(void)
{
	delay_ms(125);
    IRYJM = 0;
 
	Sys_Init();
	ULTR_Init();
	Data_Init();
    UART_Init();
    
	T0IE = 1;     		 //�_���r��0�Д�
	GIE = 1;       	//���Д�
	while(!IRYJM)	{if(Count >= 65535) break;}
    T0IE = 0;     		 //�ض��r��0�Д�
    
	UART_RX();
    Count = 0;

	//��ѭ��
	while(1)     //����ʼ�����ִ̨��
	{ 
		//T0IE = 1;      //�_���r��0�Д�
		GIE = 1;       //���Д�
		ClrWdt(); 		//�忴�Ź�
        
		//���ԡ�����ʱ����
//        TRIG ^= 1;
//        delay_ms(500);
//		ULTR_Work();
//		delay_ms(200);
		
//		//���ԡ�����ʱ������
//		if(Per10ms)
//        {
//			Per10ms = 0;
//            TRIG ^= 1;
//		}
		
//		//���ԡ���������
//		if(ECHO)	TRIG = 1;
//        else 		TRIG = 0;

        ULTR_Work();
		delay_ms(25);
    }
}