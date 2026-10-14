#include "IIC.h"
#include "stm32f10x.h"                  // Device header

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1



//**********************************引脚配置层*************************************//


/******************************************************************************
 * 函数名称: I2c_delay
 * 函数功能: I2c 延时函数
 * 入口参数: 无
 * 返回值：
 * 备注: 无
 ******************************************************************************/
void I2c_delay()  
{
    volatile unsigned char i = 2;
    while (i)
	{
		i--;
	}   
}

/******************************************************************************
 * 函数名称: IIC_Init
 * 函数功能: 软件I2c  GPIO初始化
 * 入口参数: 无
 * 返回值：
 * 备注: 无
 ******************************************************************************/
void IIC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Pin = SCL_PIN | SDA_PIN;//(PB6,PB7)
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}






//**********************************协议层*************************************//





/******************************************************************************
 * 函数名称: IIC_START（只允许在本源文件内调用）
 * 函数功能: IIC起始信号  
 * 入口参数: 无
 * 返回值：0表示成功，1表示失败
 * 备注: SCL高电平期间，SDA从高电平切换到低电平
 ******************************************************************************/
static  uint8_t IIC_START(void)
{
	SDA_H;//释放SDA，确保SDA为高电平
	SCL_H;//释放SCL，确保SCL为高电平
	
	I2c_delay();
	
	if (!SDA_Read) //如果SDA是低电平，说明被占用
        return FAILED;
	
	SDA_L;//在SCL高电平期间，拉低SDA，产生起始信号
	
	I2c_delay();
	
	if (SDA_Read)//如果SDA是高电平，说明没有成功拉低
        return FAILED;

	SCL_L;//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
	
	I2c_delay();
	return SUCCESS;
}


/******************************************************************************
 * 函数名称: IIC_STOP（只允许在本源文件内调用）
 * 函数功能: IIC终止信号  
 * 入口参数: 无
 * 返回值：无
 * 备注: 默认SDA和SCL都是低电平，先拉高SCL再拉高SDA
 ******************************************************************************/
static void IIC_Stop(void)
{
	SCL_L;//保证SCL是低电平
    I2c_delay();
	
    SDA_L;//保证SDA是低电平
	I2c_delay();
    I2c_delay();
	
    SCL_H;//先拉高SCL
	I2c_delay();
	
    SDA_H;//再拉高SDA
    I2c_delay();	
}

/******************************************************************************
 * 函数名称: IIC_SendByte
 * 函数功能: 主机发送一个字节数据  
 * 入口参数: uint8_t byte
 * 返回值：无
 * 备注: SCL低电平放数据，SCL高电平从机读
 ******************************************************************************/
static void IIC_SendByte(uint8_t byte)
{
	uint8_t i = 8;
	while(i--)		//循环8次
	{
		SCL_L;		//SCL低电平放入数据
		I2c_delay();
		
		if(byte & 0x80)//判断高位是否是1，如果是，SDA拉高
			SDA_H;
		else			//如果是0，SDA拉低
			SDA_L;
		
		byte <<= 1;		//左移一位
		
		SCL_H;			//将SCL拉高，读取数据
		I2c_delay();
		I2c_delay();
		I2c_delay();
	}
	SCL_L;				//完成一个循环，拉低SCL继续占用
}

/******************************************************************************
 * 函数名称: IIC_ReadByte
 * 函数功能: 主机接收一个字节数据  
 * 入口参数: 无
 * 返回值：读取的数据
 * 备注: SCL高电平读取，SCL低电平写入
 ******************************************************************************/
static uint8_t IIC_ReadByte(void)
{
	uint8_t i = 8 , byte = 0;
	SDA_H;        //主机释放SDA给从机
	while(i--)
	{
		byte <<=1;	// 每次循环先将整个字节左移一位
		SCL_L;
		I2c_delay();
		I2c_delay();
		SCL_H;	
		I2c_delay();
		I2c_delay();
		I2c_delay();
		if(SDA_Read)
		{
			byte |= 0x01;    //将新读到的位放在当前字节的最低位
		}
	}
	SCL_L;					//完成一个循环，拉低SCL继续占用
	return byte;
}


/******************************************************************************
 * 函数名称: IIC_Ack
 * 函数功能: 产生应答信号  
 * 入口参数: 无
 * 返回值：无
 * 备注: 0表示发送应答
 ******************************************************************************/
static void IIC_Ack(void)
{
    SCL_L;			//SCL低电平写入数据
    I2c_delay();
	
    SDA_L;			//拉低SDA写入0
    I2c_delay();
	
    SCL_H;			//SCL高电平从机读取
	I2c_delay();
	I2c_delay();
    I2c_delay();
	
    SCL_L;			//拉低SCL继续占用
    I2c_delay();
}

/******************************************************************************
 * 函数名称: IIC_NAck
 * 函数功能: 产生非应答信号  
 * 入口参数: 无
 * 返回值：无
 * 备注: 1表示发送非应答
 ******************************************************************************/
static void IIC_NAck(void)
{
    SCL_L;			//SCL低电平写入数据
    I2c_delay();
	
    SDA_H;			//拉高SDA写入1
    I2c_delay();
	
    SCL_H;			//SCL高电平从机读取
	I2c_delay();
	I2c_delay();
    I2c_delay();
	
    SCL_L;			//拉低SCL继续占用
    I2c_delay();
}

/******************************************************************************
 * 函数名称: IIC_WaitAck
 * 函数功能: 主机等待应答信号  
 * 入口参数: 无
 * 返回值：1，接收应答失败
 *         0，接收应答成功
 * 备注: 
 ******************************************************************************/
static uint8_t IIC_WaitAck(void)
{
	SCL_L;			//确保SCL处于空闲状态
    I2c_delay();
	
    SDA_H;			//释放SDA，给从机时间写入
    I2c_delay();
	
    SCL_H;			//在SCL高电平期间读取SDA
	I2c_delay();
	I2c_delay();
    I2c_delay();
	
    if (SDA_Read) 
	{
        SCL_L;
        return FAILED;
    }
    SCL_L;
    return SUCCESS;
}

//**********************************应用层*************************************//

/******************************************************************************
 * 函数名称: IIC_Write_Bytes
 * 函数功能: IIC向设备的某一个地址写入一个固定长度的数据
 * 入口参数: addr,     设备地址
 *           reg，     寄存器地址
 *			 len，     数据长度
 *			 *data	   数据指针
 * 返回值：0成功，1失败
 * 备注: 
 ******************************************************************************/
int8_t IIC_Write_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len)
{
    int i;
    if (IIC_START() == FAILED)//执行起始信号，如果失败，返回1
        return FAILED;
	
    IIC_SendByte(addr);//发送设备地址
    if (IIC_WaitAck() == FAILED) //执行等待应答信号，如果回1，则说明接收应答
	{
        IIC_Stop();//停止信号
        return FAILED;//失败，返回1
    }
	
    IIC_SendByte(reg);//发送寄存器地址
    IIC_WaitAck();//等待应答信号
    for (i = 0; i < len; i++) 
	{
        IIC_SendByte(data[i]);//将数据包内的数据逐个发送
		
        if (IIC_WaitAck() == FAILED)//等待应答信号
		{
            IIC_Stop();
            return FAILED;
        }
    }
    IIC_Stop();
    return SUCCESS;
}

/*****************************************************************************
 *函数名称:	IIC_Write_One_Byte
 *函数功能:	写入指定设备 指定寄存器一个字节
 *入口参数： addr 目标设备地址
 *		     reg   寄存器地址
 *		     data 读出的数据将要存放的地址
 *******************************************************************************/
int8_t IIC_Write_One_Byte(uint8_t addr,uint8_t reg,uint8_t data)
{
    if (IIC_START() == FAILED)//执行起始信号，如果失败，返回1
        return FAILED;
	
    IIC_SendByte(addr);//发送设备地址
	
    if (IIC_WaitAck() == FAILED)//识别到非应答
	{
        IIC_Stop();//执行终止信号
        return FAILED;
    }
    IIC_SendByte(reg);//发送寄存器地址
    IIC_WaitAck();//等待应答
    IIC_SendByte(data);//发送数据
    IIC_WaitAck();//等待应答
    IIC_Stop();//执行终止信号
    return SUCCESS;
}


/*****************************************************************************
 *函数名称:	IIC_read_Bytes
 *函数功能:	读取指定设备 指定寄存器固定长度的数据
 *入口参数： addr,     设备地址
 *           reg，     寄存器地址
 *			 len，     数据长度
 *			 *data	   数据指针
 *******************************************************************************/
int8_t IIC_read_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len)
{
    if (IIC_START() == FAILED)//执行起始信号，如果失败，返回1
        return FAILED;
	
    IIC_SendByte(addr);//发送设备地址
    if (IIC_WaitAck() == FAILED) //执行等待应答，如果失败，返回1
	{
        IIC_Stop();
        return FAILED;
    }
	
    IIC_SendByte(reg);//发送寄存器地址
    IIC_WaitAck();//等待应答
	
	IIC_Stop();//终止信号
	
    IIC_START();//重复起始条件，从写模式切换到读模式
    IIC_SendByte(addr+1);//从设备地址加上读方向位(相当于将地址左移一位后最低比特位置1),告诉设备要开始读数据
	
    if (IIC_WaitAck() == FAILED) 
	{
        IIC_Stop();
        return FAILED;
    }
	
    while (len) 
	{
        *data = IIC_ReadByte();//读取一个字节的数据，并存储到提供的缓冲区中
        if (len == 1)//最后一个字节
            IIC_NAck();//发送非应答信号
        else
            IIC_Ack();//不是最后一个字节，发送应答信号
        data++;//更新数据指针 
        len--;
    }
    IIC_Stop();//停止，释放总线
    return SUCCESS;
}

/*****************************************************************************
 *函数名称:	IIC_Read_One_Byte
 *函数功能:	读取指定设备 指定寄存器固定长度的数据
 *入口参数： addr,     设备地址
 *           reg，     寄存器地址
 *******************************************************************************/
int8_t IIC_Read_One_Byte(uint8_t addr,uint8_t reg)
{
	uint8_t recive = 0;			//设置存储变量
	
    if (IIC_START() == FAILED)	//执行等待应答，如果失败，返回1
        return FAILED;
	
    IIC_SendByte(addr);			//发送设备地址
	
    if (IIC_WaitAck() == FAILED)//执行等待应答，如果失败，返回1
	{
        IIC_Stop();
        return FAILED;
    }
	
    IIC_SendByte(reg);			//发送寄存器地址
    IIC_WaitAck();				//等待应答
	IIC_Stop();					//终止信号
    IIC_START();				//重复起始条件，从写模式切换到读模式
	IIC_SendByte(addr+1);		//从设备地址加上读方向位(相当于将地址左移一位后最低比特位置1),告诉设备要开始读数据
	
    if (IIC_WaitAck() == FAILED) 
	{
        IIC_Stop();
        return FAILED;
    }
	
	recive = IIC_ReadByte();	//读取数据
	IIC_NAck();					//产生非应答
	IIC_Stop();					//停止信号
	return recive;
}
