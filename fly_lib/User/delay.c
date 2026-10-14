#include "stm32f10x.h"
#include "misc.h"
#include "delay.h"
#include "ALL_DATA.h"


static volatile uint32_t usTicks = 0;//可被中断或其他外部机制修改，防止优化导致读取旧值


/******************************************************************************
 * 函数名称: cycleCounterInit
 * 函数功能: 时钟初始化函数
 * 入口参数: 无
 * 返回值：无
 * 备注: 初始化usTicks，计算每微秒的系统时钟周期数
 ******************************************************************************/
void cycleCounterInit(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);//获取当前配置的时钟频率
    usTicks = RCC_Clocks.SYSCLK_Frequency / 1000000;//精确一微秒的时间周期
}


/******************************************************************************
 * 函数名称: GetSysTime_us
 * 函数功能: 微秒时间戳的获取
 * 入口参数: 无
 * 返回值：无
 * 备注: 初始化usTicks，计算每微秒的系统时钟周期数
 * 		(ms * 1000)：将已过去的整毫秒数转换为微秒。
 * 		(usTicks * 1000)：计算出一个整毫秒周期内对应的总时钟周期数。
 * 		(usTicks * 1000 - cycle_cnt)：由于SysTick是递减计数器，这个计算得到的是当前毫秒周期内已经流逝的时钟周期数。
 * 		(usTicks * 1000 - cycle_cnt) / usTicks：将已流逝的周期数转换为微秒数。
 ******************************************************************************/
uint32_t GetSysTime_us(void) 
{
    register uint32_t ms, cycle_cnt;	//提示编译器将变量存储在寄存器重以提高访问速度
	
    do {
			ms = SysTick_count;			//读取的毫秒数
			cycle_cnt = SysTick->VAL;	//SysTick定时器的当前计数值赋值给cycle_cnt
    	} while (ms != SysTick_count);	//
	
    return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}


/******************************************************************************
 * 函数名称: delay_ms
 * 函数功能: 实现了基于时间戳比较的非阻塞延时
 * 入口参数: uint16_t nms延时时间
 * 返回值：无
 * 备注: 记录下开始时间，不断比较是否达到了所需要的毫秒数
 ******************************************************************************/ 
void delay_ms(uint16_t nms)
{
	uint32_t t0=GetSysTime_us();
	while(GetSysTime_us() - t0 < nms * 1000);	  	  
}


/******************************************************************************
 * 函数名称: delay_us
 * 函数功能: 执行预定次数的空循环来消耗时间
 * 入口参数: 无
 * 返回值：无
 * 备注: 初始化usTicks，计算每微秒的系统时钟周期数
 ******************************************************************************/ 
void delay_us(unsigned int i)
 {  
	char x=0;	 
    while(i--)
    {
       for(x=1;x>0;x--);
    }
 }		  

