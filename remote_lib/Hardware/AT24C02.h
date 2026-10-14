#ifndef _AT24C02_H_
#define _AT24C02_H_

#include "stm32f10x.h"  

#define ROLL_ADDR_OFFSET 0//ºá¹ö½Ç´æ´¢µØÖ·
#define PITCH_ADDR_OFFSET 4//¸©Ñö½Ç´æ´¢µØÖ·


uint16_t Read_AT24C02(uint8_t addr);
void Write_AT24C02(uint8_t addr, uint16_t data);
uint8_t Test_AT24C02(void);



#endif

