#ifndef _ADC_DMA_Config_H
#define _ADC_DMA_Config_H

#include "stm32f10x.h"

extern __IO uint16_t ADC_ConvertedValue[4];
void ADC1_DMA_Config(void);

#endif

