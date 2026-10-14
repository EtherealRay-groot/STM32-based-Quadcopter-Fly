#include "ALL_DEFINE.h"

volatile uint32_t SysTick_count; //系统时间计数
_st_Mpu MPU6050;   //MPU6050原始数据（这个变量用于在内存中开辟一块空间，专门用来存放从MPU6050传感器读取到的原始数据）
_st_AngE Angle;    //当前角度姿态值
_st_Remote Remote; //遥控通道值


_st_ALL_flag ALL_flag; //系统标志位，包含解锁标志位等


PidObject pidRateX; //内环PID数据
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; //外环PID数据
PidObject pidRoll;
PidObject pidYaw;

int16_t motor_PWM_Value[4];
void pid_param_Init(void);

/******************************************************************************
 * 函数名称: ALL_INIT
 * 函数功能: 全部外设初始化
 * 入口参数: 
 * 返回值：
 * 备注:
 ******************************************************************************/
void ALL_INIT(void)
{
	IIC_Init(); //软件IIC软件初始化
	
	pid_param_Init();
	delay_ms(200);
	
	MPU6050_Init();//MPU6050初始化
	
//----------------------------------------	
// 水平静止标定，该功能只需要进行一次，不要每次进行。店家发货前已经进行一次标定了，标定完后会自动保存到MCU的FLASH中。
// 如需校准，重新打开即可，延时5S是为了插上电池后有充足的时间将飞行器放在地上进行水平静止标定。
//	delay_ms(5000);
//  MpuGetOffset();
//----------------------------------------	
	
	Serial_Init();//串口初始化
//	OLED_Init();//OLED初始化
	
	NRF24L01_Init();//初始化NRF24L01并设置为接收模式
	
	TIM3_LED_PWM_Config();//TIM3（LED）输出比较初始化
	TIM2_MOTOR_PWM_Config();//TIM2（Motor）输出比较初始化
	
	TIM1_Config();//TIM1定时中断初始化
}


/******************************************************************************
 * 函数名称: pid_param_Init
 * 函数功能: 设置pid参数
 * 入口参数: 
 * 返回值：
 * 备注:
 ******************************************************************************/
void pid_param_Init(void)
{
	pidRateX.kp = 3.f;
	pidRateY.kp = 3.f;
	pidRateZ.kp = 6.0f;
	
	pidRateX.kd = 0.24f;
	pidRateY.kd = 0.24f;
	pidRateZ.kd = 0.3f;	
	
	pidPitch.kp = 10.0f;
	pidRoll.kp = 10.0f;
	pidYaw.kp = 8.0f;	
	
}
