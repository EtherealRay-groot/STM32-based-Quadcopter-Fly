#ifndef _DELAY_H
#define _DELAY_H 


#include "sys.h"


extern volatile uint32_t TIMIRQCNT;

void cycleCounterInit(void);
void delay_ms(uint16_t nms);
uint32_t GetSysTime_us(void); 
void delay_us(unsigned int i);

#endif
