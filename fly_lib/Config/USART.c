#include "usart.h"
#include "misc.h"
#include "stdio.h"
#include "delay.h"


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx;			//模式，选择为发送模式
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}






















//uint8_t datatest[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
//	
///******************************************************************************
// * 函数名称: USART1_DMA_Init
// * 函数功能: USART1和DMA1初始化
// * 入口参数: 
// * 返回值：
// * 备注:
// ******************************************************************************/	
//void USART1_DMA_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;	
//	
//	/* 配置串口1 （USART1）、DMA1时钟*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
//	

//	/*设置PA9为推挽输出，PA10为浮空输入*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//TX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//RX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	//USART 初始化设置
//	USART_InitStructure.USART_BaudRate = 230400;//串口波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
//	USART_Init(USART1, &USART_InitStructure);
//	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
//	
//	/* 使能串口1中断 */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//USART1  串口1全局中断 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //子优先级1
//	/*IRQ通道使能*/
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	/*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1*/
//	NVIC_Init(&NVIC_InitStructure);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // 发送DMA通道的中断配置
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // 优先级设置
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);	
//	
//    //DMA初始化设置
//    DMA_Cmd(DMA1_Channel4, DISABLE); 	// 关DMA通道  USART1_TX   DMA1_Channel4
//    DMA_DeInit(DMA1_Channel4); // 恢复缺省值
//	
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);		// 设置串口发送数据寄存器
//    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;         			// 设置发送缓冲区首地址，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 设置外设位目标，内存缓冲区 -> 外设寄存器
//    DMA_InitStructure.DMA_BufferSize = 0;                     				// 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
//    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;       

//	// 关闭内存到内存的DMA模式
//    DMA_Init(DMA1_Channel4, &DMA_InitStructure);// 写入配置
//    DMA_ClearFlag( DMA1_FLAG_GL4/*DMA1_FLAG_TC4 | DMA1_FLAG_TE4 */);// 清除DMA所有标志
//	DMA_Cmd(DMA1_Channel4, DISABLE); // 关闭DMA
//    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);//开启发送DMA通道中断
//	
//	USART_Cmd(USART1, ENABLE); //使能串口 
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);//开启串口DMA发送
//}

//void DMA1_Channel4_IRQHandler(void)
//{

//    if(DMA_GetITStatus(DMA1_FLAG_TC4))
//    {
//		DMA_ClearFlag( DMA1_FLAG_GL4/*DMA1_FLAG_TC4 | DMA1_FLAG_TE4 */);

//		DMA_Cmd(DMA1_Channel4, DISABLE);   // 关闭DMA通道
//    }

//}


//void Uart_Start_DMA_Tx(const int8_t *data ,uint8_t size)
//{
////		DMA1_Channel4->CCR &= ~DMA_CCR1_EN;
////delay_ms(1);

//	DMA1_Channel4->CPAR = (uint32_t)(&USART1->DR);
//	DMA1_Channel4->CMAR = (uint32_t)data;
//	
//    DMA1_Channel4->CNDTR = (uint16_t)size; // 设置要发送的字节数目

//  //  DMA_Cmd(DMA1_Channel4, ENABLE);        //开始DMA发送
//	DMA1_Channel4->CCR |= DMA_CCR1_EN;
//}



//void USART1_SendByte(const int8_t *Data,uint8_t len)
//{ 
//	uint8_t i;
//	
//	for(i=0;i<len;i++)
//	{
//		while (!(USART1->SR & USART_FLAG_TXE));	 // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//		USART_SendData(USART1,*(Data+i));	 
//	}
//}




//int8_t CheckSend[7]={0xAA,0XAA,0xEF,2,0,0,0};

//void USART1_setBaudRate(uint32_t baudRate)
//{
//	USART_InitTypeDef USART_InitStructure;
//	USART_InitStructure.USART_BaudRate =  baudRate;
//	USART_Init(USART1, &USART_InitStructure);
//}






///*
// * 函数名：fputc
// * 描述  ：重定向c库函数printf到USART1
// * 输入  ：无
// * 输出  ：无
// * 调用  ：由printf调用
// */
//int fputc(int ch, FILE *f)
//{
//	/* 将Printf内容发往串口 */
//	USART_SendData(USART1, (unsigned char) ch);
//	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
//	return (ch);
//}

