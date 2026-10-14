#include "NRF24L01.h"
#include "stm32f10x.h"                  // Device header
#include "SPI.h"
#include "ALL_DEFINE.h"


const uint8_t TX_ADDRESS[] = {0xE1,0xE2,0xE3,0xE4,0xE5};//发送地址（本机地址）
const uint8_t RX_ADDRESS[] = {0xE1,0xE2,0xE3,0xE4,0xE5};//接收地址


/******************************************************************************
 * 函数名称: NRF24L01_Init
 * 函数功能: NRF24L01所有引脚初始化
 * 入口参数: 无
 * 返回值：无
 * 备注: 取消片选，进入待机模式，SCK空闲为低电平，MOSI默认高电平
 ******************************************************************************/
void NRF24L01_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//初始化CSN和CE为推挽输出
	GPIO_InitStructure.GPIO_Pin = CSN | CE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB , &GPIO_InitStructure);
	
	CE_H;//进入收发模式
	CSN_H;//取消片选
	
	GPIO_InitStructure.GPIO_Pin = IRQ;//配置IRQ为上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     		//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_Config();//SPI通信引脚初始化
	
	CE_L;//进入待机模式
	CSN_H;//取消片选
}

/******************************************************************************
 * 函数名称: NRF24L01_Write_Reg（无误）
 * 函数功能: 对NRF24L01指定寄存器写入一个字节数据
 * 入口参数: RegAddr：寄存器地址、Data：写入数据
 * 返回值：状态寄存器值
 * 备注: 
 ******************************************************************************/
uint8_t NRF24L01_Write_Reg(uint8_t RegAddr,uint8_t Data)
{
	uint8_t state;
	
	CSN_L;//选中从机
	state = SPI_RW(SPI_WRITE_REG | RegAddr);//对指定寄存器执行写操作，同时返回状态寄存器值
	SPI_RW(Data);//写入指定数据
	CSN_H;//释放从机
	return state;
}


/******************************************************************************
 * 函数名称: NRF24L01_Read_Reg（无误）
 * 函数功能: 对NRF24L01指定寄存器读一个字节数据
 * 入口参数: RegAddr：寄存器地址
 * 返回值：寄存器值
 * 备注: 
 ******************************************************************************/
uint8_t NRF24L01_Read_Reg(uint8_t RegAddr)
{
	uint8_t Data;
	
	CSN_L;//选中从机
	SPI_RW(SPI_READ_REG | RegAddr);//对指定寄存器执行写操作，同时返回状态寄存器值
	Data = SPI_RW(NOP);//写入0XFF读取
	CSN_H;//释放从机
	return Data;
}

/******************************************************************************
 * 函数名称: NRF24L01_Read_Buf（无误）
 * 函数功能: 对NRF24L01指定寄存器读数据
 * 入口参数: RegAddr：寄存器地址、*Dataarray：存储数据数组、datalen：字节大小
 * 返回值：寄存器值
 * 备注: 
 ******************************************************************************/
uint8_t NRF24L01_Read_Buf(uint8_t RegAddr , uint8_t *DataArray , uint8_t datalen)
{
	uint8_t State , i;
	CSN_L;//选中从机
	State =  SPI_RW(SPI_READ_REG | RegAddr);//对指定寄存器地址执行读操作
	for(i=0; i<datalen; i++)
	{
		DataArray[i] = SPI_RW(NOP);//循环datelen次交换一个字节操作
	}
	CSN_H;//释放从机
	return State;//返回状态寄存器值
}

/******************************************************************************
 * 函数名称: NRF24L01_Write_Buf（无误）
 * 函数功能: 对NRF24L01指定寄存器写入数据
 * 入口参数: RegAddr：寄存器地址、*Dataarray：存储数据数组、datalen：字节大小
 * 返回值：寄存器值
 * 备注: 
 ******************************************************************************/
uint8_t NRF24L01_Write_Buf(uint8_t RegAddr , uint8_t *DataArray , uint8_t datalen)
{
	uint8_t State , i;
	CSN_L;//选中从机
	State =  SPI_RW(SPI_WRITE_REG | RegAddr);//对指定寄存器地址执行写操作
	for(i=0; i<datalen; i++)
	{
		SPI_RW(DataArray[i]);//循环datelen次交换一个字节操作
	}
	CSN_H;//释放从机
	return State;//返回状态寄存器值
}

/******************************************************************************
 * 函数名称: NRF24L01_TxPacket
 * 函数功能: 对NRF24L01的TX_PLOAD写入多个字节数据
 * 入口参数:  *txbuf:存储数据数组
 * 返回值：0，接收完成；其他，错误代码
 * 备注: 读取状态寄存器的值，再将该值写进状态寄存器用于清除中断标志位，同时判断是产生什么中断
 ******************************************************************************/
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t state;   
	
	CE_L;									 //退出收发模式（只有退出收发模式才能写入TX_FIFO）
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);//对TX_FIFO写数据txbuf  32个字节
	CE_H;                                    //进入收发模式	  
	
	while(Read_IRQ != 0);                    //等待IRQ产生中断：可能1（接收到应答信号会将TX_DS位置高并把数据从TX_FIFO中清除掉）
											 //					可能2（到达了最大重发次数，会将MAX_RT置高，不清除TX_FIFO中的数据）
	
	state = NRF24L01_Read_Reg(STATUS);                     //读取状态寄存器的值	   
	NRF24L01_Write_Reg(SPI_WRITE_REG | STATUS, state);     //清除TX_DS或MAX_RT或RX_DS中断标志
	
	if(state & MAX_TX)                                     //达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX, 0xff);               //清除TX_FIFO寄存器 
		return MAX_TX; 									  //返回0x10
	}
	if(state & TX_OK)                                      //发送完成
	{
		return SUCCESS;									   
	}
	return FAILED;                                         //其他原因发送失败
}

/******************************************************************************
 * 函数名称: NRF24L01_RxPacket
 * 函数功能: 对NRF24L01的RX_PLOAD读取多个字节数据
 * 入口参数:  *rxbuf:待接收数据首地址
 * 返回值：0，接收完成；其他，错误代码
 * 备注: 读取状态寄存器的值，再将该值写进状态寄存器用于清除中断标志位，同时判断是产生什么中断
 ******************************************************************************/
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;	

	state = NRF24L01_Read_Reg(STATUS);                        //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(SPI_WRITE_REG | STATUS,state);         //清除TX_DS或MAX_RT或RX_DS中断标志
	
	if(state & RX_OK)                                         //接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);          		  //清除RX FIFO寄存器

		return SUCCESS; 
	}	
  
	return FAILED;                                      //没收到任何数据
}

/******************************************************************************
 * 函数名称: RX_Mode
 * 函数功能: 设置NRF24L01为接收模式
 * 入口参数: 无
 * 返回值：无
 * 备注: 
 ******************************************************************************/
void RX_Mode(void)
{
	CE_L;//CE置低，用于写入寄存器数据	

	NRF24L01_Write_Reg(SPI_WRITE_REG | CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP(1)上电, EN_CRC(1)CRC使能, 16BIT_CRC校验, PRIM_RX接收模式  
  	NRF24L01_Write_Reg(SPI_WRITE_REG | EN_AA, 0x00); //关闭所有通道的自动应答（无自动重传）
  	NRF24L01_Write_Reg(SPI_WRITE_REG | EN_RXADDR, 0x01);//使能通道0的接收地址
	NRF24L01_Write_Reg(SPI_WRITE_REG | SETUP_AW, 0x03);//接收地址宽度5字节
	NRF24L01_Write_Reg(SPI_WRITE_REG | SETUP_RETR, 0x03);//设置自动重传，间隔250us，重传3次
  	NRF24L01_Write_Reg(SPI_WRITE_REG | RF_CH, 1);//设置RF通信频率，2401MHz(2400 + 1)
	NRF24L01_Write_Reg(SPI_WRITE_REG | RF_SETUP, 0x07);//设置TX发射参数, 0db增益, 1Mbps, 低噪声增益开启
  	NRF24L01_Write_Reg(SPI_WRITE_REG | RX_PW_P0, RX_PLOAD_WIDTH);//设置接收通道0的有效数据宽度32字节
	NRF24L01_Write_Buf(SPI_WRITE_REG | RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);//写RX节点地址

  	CE_H;//CE为高,进入接收模式                                 
}	

		   

/******************************************************************************
 * 函数名称: NRF24L01_Check
 * 函数功能: 上电检测NRF24L01是否在位
 * 入口参数: 无
 * 返回值：相同时返回值:0，表示在位;否则返回1，表示不在位	
 * 备注: 对TX_ADDR寄存器写五个字节地址再读出来，看看是不是一样的
 ******************************************************************************/
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t buf1[5];
	uint8_t i;   	 
	NRF24L01_Write_Buf(SPI_WRITE_REG | TX_ADDR, buf, 5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf1,5);              //读出写入的地址 
 	
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0XA5)
			break;
	}	
	
	if(i == 5)
		return SUCCESS;//NRF24L01在位
	else
		return FAILED;	//NRF24L01不在位	                                
}	 	 


/******************************************************************************
 * 函数名称: NRF24L01_Init
 * 函数功能: NRF24L01初始化
 * 入口参数: 
 * 返回值：
 * 备注: 默认为接受模式
 ******************************************************************************/
void NRF24L01_Init(void)
{
	NRF24L01_Config();
  do
	{
		RX_Mode();
	}
	while(NRF24L01_Check() == FAILED);
}



///******************************************************************************
// * 函数名称: TX_Mode
//* 函数功能: 设置NRF24L01为发送模式
// * 入口参数: 无
// * 返回值：无
// * 备注: CE为高大于10us,启动发送.
// ******************************************************************************/
//void TX_Mode(void)
//{														 
//	CE_L;	    
//   
//  	NRF24L01_Write_Buf(SPI_WRITE_REG | TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);//写TX节点地址     
//  	NRF24L01_Write_Buf(SPI_WRITE_REG | RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH);//设置TX节点地址,主要为了使能ACK 
//	
//  	NRF24L01_Write_Reg(SPI_WRITE_REG | EN_AA, 0x01);//使能通道0的自动应答     
//  	NRF24L01_Write_Reg(SPI_WRITE_REG | EN_RXADDR, 0x01);//使能通道0的接收地址 
//  	NRF24L01_Write_Reg(SPI_WRITE_REG | SETUP_RETR, 0x1a); //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
//    	NRF24L01_Write_Reg(SPI_WRITE_REG | RF_CH, 40);//设置RF通道为40 即2.440GHz      
//  	NRF24L01_Write_Reg(SPI_WRITE_REG | RF_SETUP, 0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
//  	NRF24L01_Write_Reg(SPI_WRITE_REG | CONFIG, 0x0e); //配置基本工作模式的参数;PWR_UP, EN_CRC, 16BIT_CRC, PRIM_RX发送模式,开启所有中断    
//    
//	CE_H;// CE为高,10us后启动发送                                  
//}	

