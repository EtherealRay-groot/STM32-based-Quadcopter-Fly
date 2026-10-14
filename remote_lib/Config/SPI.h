#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x.h"


#define SCK  		GPIO_Pin_13//时钟控制线
#define MOSI		GPIO_Pin_15//主机输出从机输入
#define MISO 		GPIO_Pin_14//主机输入从机输出
#define CE  		GPIO_Pin_8  //使能线：CE对应PB8（发送和接收时都要拉高）
#define IRQ   		GPIO_Pin_8  //IRQ对应PA8
#define CSN 		GPIO_Pin_12//片选控制线（低电平有效）

#define SCK_H        GPIO_SetBits(GPIOB , SCK)			//SCK置高
#define SCK_L        GPIO_ResetBits(GPIOB , SCK)		//SCK置低

#define MOSI_H  	 GPIO_SetBits(GPIOB, MOSI)  		//MOSI置高
#define MOSI_L  	 GPIO_ResetBits(GPIOB, MOSI)		//MOSI置低

#define Read_MISO    GPIO_ReadInputDataBit(GPIOB, MISO)  //读取MISO

#define CE_H		 GPIO_SetBits(GPIOB, CE) 		 	 //CE置高 
#define CE_L		 GPIO_ResetBits(GPIOB, CE) 		 	 //CE置低

#define Read_IRQ     GPIO_ReadInputDataBit(GPIOA, IRQ) //读取IRQ输出电平

#define CSN_H		 GPIO_SetBits(GPIOB, CSN) 		 	 //CS置高 
#define CSN_L		 GPIO_ResetBits(GPIOB, CSN) 		 //CS置低


void SPI_Config(void);
uint8_t SPI_RW(uint8_t byte);

#endif

