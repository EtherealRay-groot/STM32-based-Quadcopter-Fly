#include "LED.h"
#include "stm32f10x.h"                  // Device header
#include "ALL_DATA.h"

sLED LED = {300,AllFlashLight}; //默认300ms闪烁一次

void Pilot_LED(void)
{
	static uint32_t LastTime = 0;// 静态变量，记录上次执行时间
	
	if(SysTick_count - LastTime > LED.FlashTime)//是否达到300ms
	{
		LastTime = SysTick_count;//记录当前值
		
		switch(LED.status)
		{
			case AlwaysOff: //常暗
			{
				Right_Front_LED1_H();
				Left_Front_LED3_H();
				Rigth_Black_LED2_H();
				Left_Black_LED4_H();
				
				break;
			}
			
			case AllFlashLight:	//一起闪烁
			{
				Right_Front_LED1_Toggle();
				Left_Front_LED3_Toggle();
				Rigth_Black_LED2_Toggle();
				Left_Black_LED4_Toggle();
				
				break;
			}
			
			case AlwaysOn://常亮
			{
				Right_Front_LED1_L();
				Left_Front_LED3_L();
				Rigth_Black_bLED2_L();
				Left_Black_LED4_L();
				
				break;
			}
			
			default://都不满足
			{
				LED.status = AlwaysOff;//默认关闭
				break;
			}
			
		}/*  END SWITCH  */
		
	}/*  END IF  */
	
}

