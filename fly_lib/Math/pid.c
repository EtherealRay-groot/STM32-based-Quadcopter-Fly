#include "pid.h"
#include "myMath.h"	
#include "stm32f10x.h"                  // Device header

/******************************************************************************
 * 函数名称: pidRest
 * 函数功能: 批量复位PID函数
 * 入口参数:PidObject **pid 指向指针的指针（指向数组），const uint8_t len 需要重置的PID控制器的数量
 * 返回值：无
 * 备注:
 ******************************************************************************/
void pidRest(PidObject **pid, const uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
	  	pid[i]->integ = 0;//清零误差积分
	    pid[i]->prevError = 0;//清零上次误差
	    pid[i]->out = 0;//清零输出值
	}
}

/******************************************************************************
 * 函数名称: pidUpdate
 * 函数功能: 更新pid参数
 * 入口参数:PidObject* pid 结构体参数， const float dt
 * 返回值：
 * 备注:pid计算公式：输出=Kp*误差+Ki*误差积分+Kd*误差微分
 ******************************************************************************/
void pidUpdate(PidObject* pid, const float dt)
{
    pid->error = pid->desired - pid->measured; //误差=目标值-实际值
    pid->integ +=  pid->error * dt;	 //误差积分累加值
    pid->deriv = ( pid->error - pid->prevError )/dt;  //前后两次误差做微分
    pid->out = pid->kp *  pid->error + pid->ki * pid->integ + pid->kd * pid->deriv;//PID输出
    pid->prevError = pid->error;  //更新上次的误差
}


/******************************************************************************
 * 函数名称: CascadePID
 * 函数功能: 设置串级pid
 * 入口参数:PidObject* pid 结构体参数， const float dt
 * 返回值：
 * 备注:pid计算公式：输出=Kp*误差+Ki*误差积分+Kd*误差微分
 ******************************************************************************/
void CascadePID(PidObject* pidRate, PidObject* pidAngE, const float dt)  //串级PID
{	 
	pidUpdate(pidAngE,dt);    //先计算外环
	
	pidRate->desired = pidAngE->out;//把外环的输出值当做内环的目标值
	
	pidUpdate(pidRate,dt);    //再计算内环	
}
