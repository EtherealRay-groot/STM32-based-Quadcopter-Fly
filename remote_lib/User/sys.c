#include "sys.h"
#include "ADC_DMA.h"
#include "SPI.h"
#include "nrf24l01.h"
#include "TIM.h"
#include "USART.h"
#include "AT24C02.h"
#include "remote.h"
#include <string.h>
#include "IIC.h"
#include "BEEP.h"
#include "delay.h"
#include "Key.h"
#include "OLED.h"


uint32_t SysTick_count = 0;

void ALL_Init(void)
{
	IIC_Init();//硬件I2C初始化
	NRF24L01_Init();//NRF24L01初始化
	Usart_Init();//串口初始化
	
	while(Test_AT24C02()!=0);//EEPROM初始化成功会跳出循环
	ADC1_DMA_Config();//ADC_DMA初始化
	
	RC_Init();//将存在AT24C02的数值读出到pitch和roll
	Key_Init();//按键初始化
	
//	OLED_Init();//OLED初始化
	
	/* 开机蜂鸣器响200ms */
	BEEP_INIT(); 
	delay_ms(200);
	BEEP_L;
}

void SysTick_Handler(void)//滴答定时器1ms中断
{   
	static uint8_t Count_10ms;
	SysTick_count++;//1ms增加一次
	
	/* 10ms调用一次遥控数据解析并发送 */
	if(Count_10ms++>=10)
	{
		Count_10ms = 0;
		RC_Analy();
	}
}
