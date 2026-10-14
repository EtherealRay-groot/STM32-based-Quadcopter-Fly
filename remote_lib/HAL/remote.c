#include "stm32f10x.h" 
#include "remote.h"
#include "ADC_DMA.h"
#include "sys.h"
#include "nrf24l01.h"
#include <math.h>
#include "AT24C02.h"
#include "delay.h"
#include "key.h"
#include "BEEP.h"
#include "USART.h"


__align(4) _st_Remote Remote={1000,1000,1000,1000,1000,1000,1000,1000};//Remote结构体定义和初始化

//校准数据
struct 
{
	int16_t flag;		//校准标志位
	int16_t roll;		//横滚角
	int16_t pitch;	    //俯仰角
	int16_t thr;		//油门
	int16_t yaw;		//偏航角
}offset = {0};//初始化都为0

/******************************************************************************
 * 函数名称: RC_Analy
 * 函数功能: 遥控数据解析
 * 入口参数: 无
 * 返回值：无
 * 备注: 0.20f（缩放因子，将0-4095缩放到一个合适的范围）
		 1000（基准值油门最低、方向回中）2000（最大值）	
		 一阶无限脉冲响应（IIR）滤波器:新值=当前原始值*0.25 + 上一次滤波值*0.75
 ******************************************************************************/
void RC_Analy(void)
{
	static uint16_t last_thr,last_roll,last_pitch,last_yaw,original;//记录上一次的值，同时设置为易变参数
	
	/* 对遥控值进行解析和滤波 */
	Remote.thr = (uint16_t)(0.20f*ADC_ConvertedValue[1])+ 1000 + offset.thr;//油门输出
	original = Remote.thr;
	last_thr = Remote.thr = Remote.thr*0.25f + 0.75f*last_thr;//新值 = 0.25 * 当前原始值 + 0.75 * 上一次滤波后的值。这个公式赋予历史数据更高的权重（75%）。
//	printf("%d,%d\r\n",original,Remote.thr);
	
	Remote.pitch = (uint16_t)(0.25f*ADC_ConvertedValue[2]) +1000 + offset.pitch;//俯仰角
	last_pitch = Remote.pitch = Remote.pitch*0.25f + 0.75f*last_pitch;//滤波
//	printf("%d\r\n",Remote.pitch);
	
	Remote.yaw 	= (uint16_t)(0.25f*ADC_ConvertedValue[0])+ 1000 + offset.yaw;//偏航角
	last_yaw = Remote.yaw = Remote.yaw*0.25f + 0.75f*last_yaw;//滤波
//	printf("%d\r\n",Remote.yaw);
	
	Remote.roll = (uint16_t)(0.25f*ADC_ConvertedValue[3]) + 1000 + offset.roll;//横滚角
	last_roll = Remote.roll = Remote.roll*0.25f + 0.75f*last_roll;//滤波
//	printf("%d\r\n",Remote.roll);
	
	/* 对遥控值进行限幅 */
	/* 油门限幅 */
	if(Remote.thr>2000)
	{
		Remote.thr = 2000;
	}	
	else if(Remote.thr<1000)
	{
		Remote.thr = 1000;
	}
	/* 俯仰角限幅 */		
	if(Remote.pitch>2000)
	{
		Remote.pitch = 2000;
	}	
	else if(Remote.pitch<1000)
	{
		Remote.pitch = 1000;
	}
	/* 横滚角限幅 */		
	if(Remote.roll>2000)
	{
		Remote.roll = 2000;
	}	
	else if(Remote.roll<1000)
	{
		Remote.roll = 1000;
	}
	/* 偏航角限幅 */	
	if(Remote.yaw>2000)
	{
		Remote.yaw = 2000;
	}
	else if(Remote.yaw<1000)
	{
		Remote.yaw = 1000;
	}
	Key_Scan();//按键扫描
	NRF_Send();//数据发送到飞控
	

}

/******************************************************************************
 * 函数名称: RC_Init
 * 函数功能: 遥控器系统初始化
 * 入口参数: 无
 * 返回值：无
 * 备注: 无
 ******************************************************************************/
void RC_Init(void)
{
	offset.roll = Read_AT24C02(ROLL_ADDR_OFFSET);//从存放横滚角的地址读取值标准值
	offset.pitch = Read_AT24C02(PITCH_ADDR_OFFSET);//从存放俯仰角的地址读取值标准值
}

/******************************************************************************
 * 函数名称: NRF_Send
 * 函数功能: 10ms发送一次数据，每个通道都是高位在前低位在后
 * 入口参数: 无
 * 返回值：无
 * 备注: (uint8_t*)&Remote将 Remote结构体的地址强制转换为字节指针，这样就可以按字节单位进行操作
		 和校验：将全部的数据累加在一起，放在帧尾发送出去
 ******************************************************************************/
uint8_t tx_data[32] = {0xAA,0xAF,0x03,32-5};//匿名通信协议（四个字节分别为两个帧头，一个识别帧，一个有效数据长度（32-5是因为一次发送32个字节，其中5个字节不是数据））

void NRF_Send(void)
{
	for(uint8_t i=4;i<26;i+=2)//从4开始，循环11次
	{
		tx_data[i] = *((uint8_t*)&Remote+i-3);	 //高位在前
		tx_data[i+1] = *((uint8_t*)&Remote+i-4);	 //低位在后
	}
	
	tx_data[31] = 0;//对校验位清零
	for(uint8_t i=0;i<31;i++)  //校验位
	{
		tx_data[31] +=  tx_data[i];//将前31位数据全部累加起来
	}
	NRF24L01_TxPacket((uint8_t*)&tx_data); //调用NRF发射数据tx_data[32]
}

/******************************************************************************
 * 函数名称: Key_Scan
 * 函数功能: 按键扫描
 * 入口参数: 无
 * 返回值：无
 * 备注: PA15：AUX1、PB11：AUX2、PC14：AUX3、PC15：AUX4
         PB1：front、PB0：back、PA6：left、PA7：right
		对于AUX1、AUX2、AUX3、AUX4按下一次切换一次，值只会是1000和2000其中一个，初始为1000、
		对于front、back控制offset里的pitch,范围在-250到250之间每次以5变换
		对于left、right控制offset里的roll,范围在-250到250之间每次以5变换
 ******************************************************************************/
void Key_Scan(void)
{
	volatile static uint8_t status = 0;	//状态标志为
	static uint32_t temp;	//时间戳
	
	
	switch(status)
	{
		case 0://检测是否都在高电平
		{
			if(SysTick_count - temp >30) //300ms 按键音
			{
				if(((GPIOA->IDR & (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15)) == (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15))//检测各按键输入引脚是否全都是高电平
					&& ((GPIOB->IDR & (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11)) == (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11))
					&& ((GPIOC->IDR & (GPIO_Pin_14|GPIO_Pin_15)) == (GPIO_Pin_14|GPIO_Pin_15)))
					{
						status = 1;//如果识别到都是高电平，设标志位为1
					}
				 
				BEEP_L;//蜂鸣器不响
			}
			break;//进入下一阶段
			
		}//END CASE 0;
		
		case 1://检测是否是有一个按键被按下（也就是输入低电平）
		{
			if(((GPIOA->IDR & (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15)) != (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15))//检测各按键输入引脚是否有一个是低电平
				|| ((GPIOB->IDR & (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11)) != (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11))
				|| ((GPIOC->IDR & (GPIO_Pin_14|GPIO_Pin_15)) != (GPIO_Pin_14|GPIO_Pin_15)))
			{
				status = 2;//如果识别到都是低电平，设标志位为2
			}
			break;//进入下一阶段
			
		}//END CASE 1;
		
		case 2:	
		{
			if(!(GPIOA->IDR & GPIO_Pin_15))//PA15输入低电平（按键AUX1按下）
			{
					Remote.AUX1 ^= (2000^1000); //在1000和2000两个值间切换（2000^1000进行异或运算得到切换掩码，再与当前值异或实现状态翻转）
			}
			else if(!(GPIOB->IDR & GPIO_Pin_11))//PB11输入低电平（按键AUX2按下）
			{
					Remote.AUX2 ^= (2000^1000); //在1000和2000两个值间切换
			}
			else if(!(GPIOC->IDR & GPIO_Pin_14))//PC14输入低电平（按键AUX3按下）
			{
					Remote.AUX3 ^= (2000^1000); //在1000和2000两个值间切换
			}
			else if(!(GPIOC->IDR & GPIO_Pin_15))//PC15输入低电平（按键AUX4按下）
			{
					Remote.AUX4 ^= (2000^1000); //在1000和2000两个值间切换
			}			
			
			else if(!(GPIOA->IDR & GPIO_Pin_6))//PA6输入低电平（按键Left按下）
			{
				if(offset.roll>-250)//确保值大于-250
				{
					offset.roll-=5;	//每次减5
					Write_AT24C02(ROLL_ADDR_OFFSET,offset.roll);//将微调校准值写入24C02	
				}				
			}
			else if(!(GPIOA->IDR & GPIO_Pin_7))//PA7输入低电平（按键Right按下）
			{
				if(offset.roll<250)//确保值小于250
				{
					offset.roll+=5;//每次加5
					Write_AT24C02(ROLL_ADDR_OFFSET,offset.roll);//将微调校准值写入24C02	
				}			
			}
			else if(!(GPIOB->IDR & GPIO_Pin_0))//PB0输入低电平（按键Front按下）
			{
				if(offset.pitch>-250)//确保值大于-250
				{
					offset.pitch-=5;//每次减5	
					Write_AT24C02(PITCH_ADDR_OFFSET,offset.pitch);//将微调校准值写入24C02		
				}
			}
			else if(!(GPIOB->IDR & GPIO_Pin_1))//PB1输入低电平（按键Back按下）
			{
				if(offset.pitch<250)//确保值小于250
				{
					offset.pitch+=5;//每次加5
					Write_AT24C02(PITCH_ADDR_OFFSET,offset.pitch);//将微调校准值写入24C02		
				}
			}
			status = 0;	//下一次从status开始执行代码		
			BEEP_H;//蜂鸣器响
			temp = SysTick_count;//记录当前的时间戳
			break;//跳出循环
			
			
		}//END CASE 2;
		
	}//END SWITCH
	
}//END FUNCTION





