#ifndef _IIC_H_
#define _IIC_H_

#include "stm32f10x.h"                  // Device header
#include "system_stm32f10x.h"

//定义引脚
#define SCL_PIN       GPIO_Pin_6
#define SDA_PIN       GPIO_Pin_7

//定义SCL线电平
#define SCL_H         GPIO_SetBits(GPIOB , GPIO_Pin_6)
#define SCL_L         GPIO_ResetBits(GPIOB , GPIO_Pin_6)

//定义SDA线电平
#define SDA_H         GPIO_SetBits(GPIOB , GPIO_Pin_7)
#define SDA_L         GPIO_ResetBits(GPIOB , GPIO_Pin_7)

//读取SCL和SDA线的电平
#define SCL_Read      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define SDA_Read      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)

extern void IIC_Init(void); 
extern int8_t IIC_Write_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len);
extern int8_t IIC_Write_One_Byte(uint8_t addr,uint8_t reg,uint8_t data);
extern int8_t IIC_read_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len);
extern int8_t IIC_Read_One_Byte(uint8_t addr,uint8_t reg);

#endif
