#include "stm32f10x.h"                  // Device header
#include "BEEP.h"
#include "RCC.h"
#include "delay.h"
#include "SPI.h"
#include "USART.h"
#include "NRF24L01.h"
#include "ADC_DMA.h"
#include "AT24C02.h"
#include "IIC.h"
#include "OLED.h"


//外部声明函数
void RCC_Configuration(void);
void NVIC_Configuration(void);


int main(void)
{
	RCC_Configuration();//配置晶振，遥控要求比较低。可以使用使用内部晶振，8M/2分频*12倍频=48MHz
	
	cycleCounterInit();//初始化每个滴答定时器的计数值，为延时函数做准备
	
	SysTick_Config(48000000 / 1000);	//配置系统滴答时钟 开启1ms发生一次中断

	ALL_Init();//所有外设硬件初始化
	
	NVIC_Configuration();//中断向量配置
	
	while(1)
	{
		
//			uint8_t i = Read_AT24C02(ROLL_ADDR_OFFSET);
//			uint8_t e = Read_AT24C02(PITCH_ADDR_OFFSET);
//			OLED_ShowNum(1, 1, e, 3);

	}
}


/*SPI时序测试程序*/
//CSN_L;
//uint8_t b = SPI_RW(0x20|0x04);
//SPI_RW(0x55);
//CSN_H;
//Usart_SendDate(b);
//		
//CSN_L;
//SPI_RW(0x00|0x04);
//uint8_t a = SPI_RW(0xFF);
//CSN_H;
//Usart_SendDate(a);

//	uint8_t a[] = {0x01,0x02,0x03,0x04,0x05};
//	uint8_t b[5];
//	
//	NRF24L01_Write_Buf(TX_ADDR, a, 5);
//	NRF24L01_Read_Buf(TX_ADDR,b,5);
//	Usart_SendArray(b,5); 

/*ADC测试程序*/
//		delay_ms(50);
//		printf("CH1:%d CH2:%d CH3:%d CH4:%d\r\n",ADC_ConvertedValue[0],ADC_ConvertedValue[1],ADC_ConvertedValue[2],ADC_ConvertedValue[3]);

/*按键测试程序*/
//	uint8_t i = Read_AT24C02(ROLL_ADDR_OFFSET);
//	uint8_t e = Read_AT24C02(PITCH_ADDR_OFFSET);
//	printf("%d,%d\r\n",offset.pitch,e);
////	printf("%d,%d\r\n",offset.roll,i);
//	printf("%d\r\n",Remote.AUX4);

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* 使能串口1中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//USART1  串口1全局中断 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //子优先级1
	/*IRQ通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1*/
	NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

