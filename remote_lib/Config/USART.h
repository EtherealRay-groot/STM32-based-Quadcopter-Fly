#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

//extern void USART1_SendByte(const int8_t *Data,uint8_t len);
//void Uart_Start_DMA_Tx(const int8_t *data ,uint8_t size);
//extern void USART1_Config(void);


void Usart_Init(void);
void Usart_SendDate(uint8_t Date);
void Usart_SendArray(uint8_t *Array , uint16_t Length); 
void Usart_SendString(char *String);
void Usart_SendNumber(uint32_t number,uint8_t Length);


#endif /* __USART1_H */

