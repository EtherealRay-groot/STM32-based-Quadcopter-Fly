#ifndef _BEEP_H___
#define _BEEP_H___

#include "stm32f10x.h"

#define BEEP_H GPIOA->BSRR = GPIO_Pin_12//·äÃùÆ÷Ïì
#define BEEP_L GPIOA->BRR = GPIO_Pin_12//·äÃùÆ÷²»Ïì


void BEEP_INIT(void);

#endif
