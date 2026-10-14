#include "stm32f10x.h"                  // Device header


void RCC_Configuration(void)
{
	// 确保使用HSI作为系统时钟源
    RCC->CR |= RCC_CR_HSION;
    while((RCC->CR & RCC_CR_HSIRDY) == 0);
    
    RCC->CFGR = RCC_SYSCLKSource_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_SYSCLKSource_HSI);
	
	
	RCC_DeInit();//将外设 RCC寄存器重设为缺省值  
  
    RCC_HSICmd(ENABLE);//使能HSI    
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);//等待HSI使能成功  
	
    // 配置Flash接口，为系统运行在更高频率下提供稳定性保障
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//开启预指令  
    FLASH_SetLatency(FLASH_Latency_2);//设置两倍延时  
      
    //设置 PLL 时钟源及倍频系数  48Mhz
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//PLL时钟来源HSI/2，同时给予12倍频
    RCC_PLLCmd(ENABLE);//使能锁相环
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待指定的 RCC 标志位设置成功 等待PLL初始化成功  
  
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置系统时钟（SYSCLK） 设置PLL为系统时钟源  
    
	//等待PLL成功用作于系统时钟的时钟源    
    while(RCC_GetSYSCLKSource() != 0x08);//需与被选择的系统时钟对应起来，RCC_SYSCLKSource_PLL  
  
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK不分频  （48MHZ）   
    RCC_PCLK1Config(RCC_HCLK_Div2); //PCLK1二分频 （24MHZ）
    RCC_PCLK2Config(RCC_HCLK_Div1); //PCLK2不分频 （48MHZ）  
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
						   |RCC_APB2Periph_ADC1  | RCC_APB2Periph_AFIO 
                           |RCC_APB2Periph_SPI1  
						   	, ENABLE );
	
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_I2C1| RCC_APB1Periph_TIM3, ENABLE );
}

