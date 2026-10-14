#include "stm32f10x.h"                  // Device header
#include "BEEP.h"


void BEEP_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;          //PA12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	BEEP_H;
}


