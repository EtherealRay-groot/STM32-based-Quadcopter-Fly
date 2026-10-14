#include "SPI.h"
#include "stm32f10x.h"                  // Device header


/******************************************************************************
 * 函数名称: SPI_Config
 * 函数功能: 软件SPI  GPIO初始化
 * 入口参数: 无
 * 返回值：无
 * 备注: CSN默认拉高，SCK空闲为低电平（对应SPI模式0），主机输出默认高电平（高低电平都可）
 ******************************************************************************/
void SPI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	CSN_H;		//片选线默认高电平，为空闲状态，不选中任何设备
	
	GPIO_InitStructure.GPIO_Pin = SCK | MOSI;				//SCK、MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MISO;				//MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //MISO用上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//配置与否不重要
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	SCK_L;			//满足CPOL为0，SPI模式0
	MOSI_H;			//主机输出高电平
}


/******************************************************************************
 * 函数名称: SPI_RW
* 函数功能: 软件SPI  发送一个字节并接收一个字节（SCK下降沿发送，上升沿读取）满足CPHA为0
 * 入口参数: 发送字节
 * 返回值：接收字节
 * 备注: 无
 ******************************************************************************/
uint8_t SPI_RW(uint8_t byte)
{
		uint8_t i;

		uint8_t Temp=0x00;

		for (i = 0; i < 8; i++)
		{		
				//SCK置低
				SCK_L;
			
				//SPI发送数据
				if (byte&0x80)
				{
					MOSI_H; 
				}
				else
				{
					MOSI_L;
				}

				byte <<= 1;
				Temp <<= 1;
				
				//拉高时钟 
				SCK_H; 
				
				if(Read_MISO) 
				{
					Temp++;
				}
				SCK_L;	
		}
		return Temp; //返回读到miso输入的值

}



