#include "stm32f10x.h"                  // Device header


/******************************************************************************
 * 函数名称: Key_Init
 * 函数功能: 按键引脚初始化
 * 入口参数: 无
 * 返回值：无
 * 备注: PA15 AUX1、PB11 AUX2、PC14 AUX3、PC15 AUX4
         PB1 front、PB0 back、PA6 left、PA7 right
 ******************************************************************************/
void Key_Init(void)
{
	//涉及到PA15
	AFIO->MAPR = 0x02000000; //不调用这个语句JTAG和SWD默认全部开启，调用后禁用JTAG，只开启SWD
	
//测试程序
//	uint32_t mapr = AFIO->MAPR;
//	printf("%x",mapr >> 24 & 0x7);
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//设置PA4，PA6，PA7，PA15为上拉输入
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//设置PB0，PB1，PB11为上拉输入
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//设置PC14，PC15为上拉输入
	
}


