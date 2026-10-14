#include "ALL_DEFINE.h"
#include "MPU6050.h"
#include "stm32f10x.h"                  // Device header
#include "IIC.h"
#include "delay.h"
#include "kalman.h"
#include "string.h"
#include "flash.h"

int16_t MpuOffset[6] = {0};//用于存放静态时的


/******************************************************************************
 * 函数名称: mpu6050_rest
 * 函数功能: mpu6050所有寄存器都恢复到默认值 
 * 入口参数: 无
 * 返回值：0成功，1失败
 * 备注: 给电源管理寄存器1写入0x80，所有寄存器恢复默认值（PWR_MGMT_1恢复为0x40）
 ******************************************************************************/
int8_t MPU6050_Rest(void)
{
	if(IIC_Write_One_Byte(MPU6050_ADDRESS, PWR_MGMT_1, 0x80) == FAILED)//写入失败返回失败
		return FAILED;	
	delay_ms(20);
	return SUCCESS;
}

/******************************************************************************
 * 函数名称: mpu6050_Init
 * 函数功能: mpu6050初始化
 * 入口参数: 无
 * 返回值：0成功，1失败
 * 备注:配置陀螺仪采样率为500Hz，设置陀螺仪z轴为时钟源，设置低通滤波，陀螺仪和加速度计量程
 ******************************************************************************/
int8_t MPU6050_Init(void)
{
	
	uint8_t date = SUCCESS;
	do
	{
		date = IIC_Write_One_Byte(MPU6050_ADDRESS, PWR_MGMT_1, 0x80);	//向电源管理寄存器写入0x80触发芯片复位
		delay_ms(30);//等待器件复位完成										
		date += IIC_Write_One_Byte(MPU6050_ADDRESS, SMPLRT_DIV, 0x02); //设置	陀螺仪采样率，0x02(333Hz=1k/3)
		date += IIC_Write_One_Byte(MPU6050_ADDRESS, PWR_MGMT_1, 0x03);	//设置设备时钟源，陀螺仪Z轴
		date += IIC_Write_One_Byte(MPU6050_ADDRESS, CONFIGL, 0x03);   //低通滤波频率，0x03(42Hz)（加速度计带宽94HZ，延时3.0ms，陀螺仪带宽98，延时2.8ms)
		date += IIC_Write_One_Byte(MPU6050_ADDRESS, GYRO_CONFIG, 0x18);//+-2000deg/s,不使能自测
		date += IIC_Write_One_Byte(MPU6050_ADDRESS, ACCEL_CONFIG, 0x08);//+-4G，不使能自测
	}
	while(date != SUCCESS);//持续赋值，直到全部写入成功
	
	//判断是否是指定的元器件
	date =IIC_Read_One_Byte(MPU6050_ADDRESS , WHO_AM_I);
	if(date != MPU6050_PRODUCT_ID)
		return FAILED;
	
	FLASH_read(MpuOffset,6);//从mcu的FLASH中读取MPU6050的水平静止标定校准值
//此处要将mcu的flash读出水平精致标定校准值
	return SUCCESS;
	
}
/******************************************************************************
 * 函数名称: MPU6050_AccRead
 * 函数功能: 读取mpu6050,三轴加速度计的实际值
 * 入口参数: 指针返回多个变量到数组
 * 返回值：无
 * 备注:
 ******************************************************************************/
void MPU6050_AccRead(int16_t *accData)
{
	uint8_t buf[6];
	IIC_read_Bytes(MPU6050_ADDRESS,ACCEL_ADDRESS,buf,6);
	accData[0] = (uint16_t)((buf[0] << 8) | buf[1]);
	accData[1] = (uint16_t)((buf[2] << 8) | buf[3]);
	accData[2] = (uint16_t)((buf[4] << 8) | buf[5]);
}

/******************************************************************************
 * 函数名称: MPU6050_GyroRead
 * 函数功能: 读取mpu6050,三轴陀螺仪的实际值
 * 入口参数: 指针返回多个变量到数组
 * 返回值：无
 * 备注:
 ******************************************************************************/
void MPU6050_GyroRead(int16_t *GyroData)
{
	uint8_t buf[6];
	IIC_read_Bytes(MPU6050_ADDRESS,GYRO_ADDRESS,buf,6);
	
	GyroData[0] = (uint16_t)((buf[0] << 8) | buf[1]);
	GyroData[1] = (uint16_t)((buf[2] << 8) | buf[3]);
	GyroData[2] = (uint16_t)((buf[4] << 8) | buf[5]);
}





/******************************************************************************
 * 函数名称: MPU6050_GetDate
 * 函数功能: 读取mpu6050数据,并对加速度做卡尔曼滤波，对角速度做一阶低通滤波
 * 入口参数: 
 * 返回值：
 * 备注:
 ******************************************************************************/
static volatile int16_t *pMpu = (int16_t *)&MPU6050;//用pMpu指针存放MPU6050结构体的地址

void MPU6050_GetDate(void)
{
	uint8_t i;
	uint8_t buffer[12];//存储读取到的原始MPU6050数据
	
	//分别将MPU6050读到的6个8位陀螺仪值，6个8位加速度计值放进buffer数组里
	IIC_read_Bytes(MPU6050_ADDRESS,ACCEL_ADDRESS,buffer,6);		//Acc_Read();
	IIC_read_Bytes(MPU6050_ADDRESS,GYRO_XOUT_H,&buffer[6],6);	//Gyro_Read();
	
	//buffer中低六位为加速度，高六位为角速度
	for(i=0;i<6;i++)
	{
			pMpu[i] = (((int16_t)buffer[i<<1] << 8) | buffer[(i<<1)+1]) - MpuOffset[i];		//将12个8bit数据整合为6个16bit，并减去水平静止校准值
			
			if(i < 3)//以下对加速度做卡尔曼滤波
			{
				{
					static struct _1_ekf_filter ekf[3] = {{0.02,0,0,0,0.001,0.543},{0.02,0,0,0,0.001,0.543},{0.02,0,0,0,0.001,0.543}};	
					kalman_1(&ekf[i],(float)pMpu[i]);  //一维卡尔曼
					pMpu[i] = (int16_t)ekf[i].out;
				}
			}/*END GYRO */
			
			if(i > 2)//以下对角速度做一阶低通滤波
			{	
				uint8_t k=i-3;
				const float factor = 0.15f;  //滤波因素，更信任过去值，平滑效果会比较好，但响应速度会稍慢一些			
				static float tBuff[3];//将上一时刻的输出滤波值存在tBuff		

				pMpu[i] = tBuff[k] = tBuff[k] * (1 - factor) + pMpu[i] * factor;  
				//一阶低通滤波公式y(n) = a*x(n) + (1-a)*y(n-1) 
				//其中x(n)代表当前时刻输入的原始值，a是滤波系数决定滤波器对新旧数据的信任度
				//y(n)代表当前时刻的输出滤波值，y(n-1)代表上一时刻的输出滤波值	
				
			}/*END ACC */
			
	}/*END FOR */
}




/******************************************************************************
 * 函数名称: Mpu6050_GetOffset
 * 函数功能: 静态零偏标定
 * 入口参数: 无
 * 返回值：无
 * 备注:在飞行器完全静止的情况下，计算6轴的零偏，并保存到flash中
 ******************************************************************************/
void Mpu6050_GetOffset(void) //校准
{
	int32_t buffer[6]={0};//累加用缓冲区
	int16_t LastGyro[3] = {0};//上一次陀螺仪数据
	int16_t ErrorGyro[3];//当前变化量
	/*若角速度变化量在±5以内，认为设备处于静止状态*/
	const int8_t MAX_GYRO_QUIET = 5;  const int8_t MIN_GYRO_QUIET = -5;	
	
	int16_t i;  
	uint8_t k=30;
	
	memset(MpuOffset,0,12);//将所有offset清零
	
//	MpuOffset[2] = 8192;   //8192对应±4g下1g的数字量
	
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);//失能定时器中断（防干扰）
	
	while(k--)//30次静止则判定飞行器处于静止状态
	{
		/*只有在静止状态下才会执行*/
		do
		{
			delay_ms(10);
			MPU6050_GetDate();//每10ms读取一次MPU6050
			
			for(i=0;i<3;i++)
			{
				ErrorGyro[i] = pMpu[i+3] - LastGyro[i];//对比前后两次的数值变化
				LastGyro[i] = pMpu[i+3];//保存本次的陀螺仪数值	
			}			
		}while ((ErrorGyro[0] >  MAX_GYRO_QUIET )|| (ErrorGyro[0] < MIN_GYRO_QUIET)//标定静止
					||(ErrorGyro[1] > MAX_GYRO_QUIET )|| (ErrorGyro[1] < MIN_GYRO_QUIET)
					||(ErrorGyro[2] > MAX_GYRO_QUIET )|| (ErrorGyro[2] < MIN_GYRO_QUIET));
	}	

/* 舍弃前100组数据，后256组数据求平均 */	
	for(i=0;i<356;i++)//水平校准
	{		
		/* 循环256次，每一次将当前的6轴数据累加到buffer里 */
		MPU6050_GetDate();//读取当前数据
		if(100 <= i)//取256组数据进行平均
		{
			uint8_t k;
			for(k=0;k<6;k++)
			{
				buffer[k] += pMpu[k];
			}
		}
	}

	for(i=0;i<6;i++)
	{
		MpuOffset[i] = buffer[i]>>8;//分别把buffer数组里的6个累加值都除256，得到最后的标定值
	}
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);//使能定时器中断 

	FLASH_write(MpuOffset,6);//将数据写到FLASH中，一共有6个int16数据
}


