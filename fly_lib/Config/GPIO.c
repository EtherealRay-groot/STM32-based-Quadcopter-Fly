#include "GPIO.h"
#include "stm32f10x.h"                  // Device header

void LED_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
}


