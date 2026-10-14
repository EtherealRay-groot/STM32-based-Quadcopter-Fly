#ifndef _DELAY_H
#define _DELAY_H 

void cycleCounterInit(void);


#include "stm32f10x.h"

extern volatile uint32_t TIMIRQCNT;



extern void cycleCounterInit(void);
extern void delay_ms(uint16_t nms);
extern uint32_t GetSysTime_us(void); 
extern void delay_us(unsigned int i);

#endif


