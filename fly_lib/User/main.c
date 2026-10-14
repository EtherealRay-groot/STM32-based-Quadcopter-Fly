#include "ALL_DEFINE.h"
#include "LED.h"
#include "NRF24L01.h"
//           机头      
//   PWM3     ♂       PWM1
//      *           *
//      	*       *
//    		  *   *
//      			* 
//    		  *   *
//      	*       *
//      *           *
//    PWM4           PWM2	

int16_t GYRO[3],ACC[3];

int main(void)
{
	cycleCounterInit();  //得到系统每个us的系统CLK个数，为以后延时函数，和得到精准的当前执行时间使用
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级
	
	SysTick_Config(SystemCoreClock / 1000);	//系统滴答时钟
	
	ALL_INIT();//所有外设、硬件初始化
	

	while(1)
	{
		
		Pilot_LED();//刷新LED状态
	}
}



//测试程序
///*MPU6050读取原始数据测试*/
//	delay_ms(50);
//	MPU6050_AccRead(ACC);
//	MPU6050_GyroRead(GYRO);
//	printf("GYRO_X:%d GYRO_Y:%d GYRO_Z:%d\r\n",GYRO[0],GYRO[1],GYRO[2]);
//	printf("ACC_X:%d ACC_Y:%d ACC_Z:%d\r\n\r\n\r\n",ACC[0],ACC[1],ACC[2]);


/*TIM3、LED测试程序*/
//	TIM3->CCR1=500;
//	TIM3->CCR2=500;
//	TIM3->CCR3=500;
//	TIM3->CCR4=500;
	

/*TIM2：电机测试程序*/
//	TIM2->CCR1=500;
//	TIM2->CCR2=500;
//	TIM2->CCR3=500;
//	TIM2->CCR4=500;
	
	
/*SPI时序测试程序*/
//	CSN_L;
//	uint8_t b = SPI_RW(0x20|0x04);
//	SPI_RW(0x55);
//	CSN_H;
//	Usart_SendDate(b);
//	
//	CSN_L;
//	SPI_RW(0x00|0x04);
//	uint8_t a = SPI_RW(0xFF);
//	CSN_H;
//	Usart_SendDate(a);



//	uint8_t a[] = {0x01,0x02,0x03,0x04,0x05};
//	uint8_t b[5];
//	
//	NRF24L01_Write_Buf(TX_ADDR, a, 5);
//	NRF24L01_Read_Buf(TX_ADDR,b,5);
//	Usart_SendArray(b,5); 

