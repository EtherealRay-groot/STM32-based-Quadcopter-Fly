#ifndef _ALL_USER_DATA_H_
#define _ALL_USER_DATA_H_

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       long long int64_t;
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       long long uint64_t;


//#define NULL 0
extern volatile uint32_t SysTick_count;

//三轴加速度，三轴角速度
typedef struct
{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
}_st_Mpu;



//姿态结算后的三个姿态参数
typedef struct
{
	float roll;
	float pitch;
	float yaw;
	
}_st_AngE;


//遥控变量
typedef struct
{
	uint16_t roll;//横滚
	uint16_t pitch;//俯仰
	uint16_t thr;//油门
	uint16_t yaw;//偏航
	uint16_t AUX1;//AUX1按键
	uint16_t AUX2;//AUX2按键
	uint16_t AUX3;//AUX3按键
	uint16_t AUX4;//AUX4按键
}_st_Remote;

//PID变量
typedef volatile struct
{
	float desired;//设置目标值
	float error;//本次误差
	float prevError;//上一次误差
	float integ;//积分项
	float deriv;//微分项
	
	float kp; //比例系数
	float ki; //积分系数
	float kd; //微分系数
	float IntegLimitHigh;//积分限幅
	float IntegLimitLow;//积分限幅
	float measured;//实际值
	float out;//输出
	float OutLimitHigh;//输出限幅
	float OutLimitLow;//输出限幅
}PidObject;

typedef volatile struct
{
	uint8_t unlock;
}_st_ALL_flag;


extern _st_Remote Remote;
extern _st_Mpu MPU6050;
extern _st_AngE Angle;


extern _st_ALL_flag ALL_flag;


extern	PidObject pidRateX;
extern	PidObject pidRateY;
extern	PidObject pidRateZ;

extern	PidObject pidPitch;
extern	PidObject pidRoll;
extern	PidObject pidYaw;

extern int16_t motor_PWM_Value[4];

#endif
