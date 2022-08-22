#include	"SYSCFG.h"
#ifndef __LED_H
#define __LED_H

#define LED    RC4

void LED_Init(void)
{
	TRISC4 = 0;	//PC4Êä³öÄ£Ê½
    LED = 1;
}

#endif

