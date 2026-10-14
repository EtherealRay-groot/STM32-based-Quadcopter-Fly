#ifndef __ADC_DMA_H 
#define __ADC_DMA_H
#include "stm32f10x.h"

#define ADC3_DR_Address    ((uint32_t)0x40013C4C)//定义ADC3的数据寄存器地址
#define ADC1_DR_Address    ((uint32_t)0x4001244C)//定义ADC1的数据寄存器地址

extern uint16_t ADC_Value[10];
extern void ADC_DMA_Init(void);
extern uint16_t GetADCValue(void);

#endif 
