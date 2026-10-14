#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "stdint.h"


#ifdef  	USE_I2C_HARDWARE//检测是否使用硬件IIC
		
	#define MPU6050_ADDRESS 0xD0//0x68
#else//使用软件IIC
	#define  MPU6050_ADDRESS 0xD0   //IIC写入时的地址字节数据，+1为读取
#endif


#define	SMPLRT_DIV		0x19	//陀螺仪采样率：【输出率/（1+SMPLRT_DIV）】	使用低通滤波时输出为1kHZ，不使用时为8kHZ		典型值：0x07(125Hz)
#define	CONFIGL			0x1A	//低通滤波频率：低三位为0和7时不使能低通滤波，外部同步设置主要是配置外部帧同步(FSYNC)引脚采样和数字，这里不做配置。		典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	ACCEL_ADDRESS	0x3B
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define GYRO_ADDRESS  	0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		  0x75	//IIC地址寄存器(默认数值0x68，只读)
#define MPU6050_PRODUCT_ID 0x68
#define MPU6052C_PRODUCT_ID 0x72

int8_t MPU6050_Rest(void);
int8_t MPU6050_Init(void);
void MPU6050_GetDate(void);

void MPU6050_AccRead(int16_t *accData);
void MPU6050_GyroRead(int16_t *GyroData);

#endif
