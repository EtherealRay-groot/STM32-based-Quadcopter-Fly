#include "Init.h"
#include "ALL_DEFINE.h"


/******************************************************************************
 * 函数名称: TIM1_UP_IRQHandler
 * 函数功能: TIM1定时器中断，3ms执行一次
 * 入口参数: 
 * 返回值：
 * 备注:
 ******************************************************************************/
void TIM1_UP_IRQHandler(void)   //TIM1中断 3ms
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		
		MPU6050_GetDate();//读取MPU6050的原始数据并进行滤波
		GetAngle(&MPU6050,&Angle,0.003f);//使用四元数结算，将三轴加速度和三轴角加速度转换成三个姿态角
		RC_Analy();	//遥控数据解析
		FlightPid_Control(0.03f);//pid控制
		MotorControl();//电机控制
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	}
}

