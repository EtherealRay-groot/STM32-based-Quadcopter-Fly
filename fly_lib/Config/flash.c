#include "flash.h"
#include "stm32f10x.h"                  // Device header

typedef enum {FAILED = 0, PASSED = !FAILED} Status;//定义要使用的状态枚举变量




uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t First_Page = 0x00;                       //起始页


__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO Status MemoryProgramStatus = PASSED;
//函数声明


/******************************************************************************
 * 函数名称: FLASH_write
 * 函数功能: 将指定数组指定长度的数据写入预定好的flash地址下
 * 入口参数: *data 指针地址后的数据，len 长度
 * 返回值：
 * 备注: 
 ******************************************************************************/
void FLASH_write(int16_t *data,uint8_t len)
{
  FLASH_Unlock();//先解锁
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清楚相应的标志位（EOP：操作完成、PGERR：编程错误、WRPRTERR：写保护错误）
//  First_Page = (FLASH_End_Addr - FLASH_Start_Addr+1) / FLASH_Page_Size;//计算出起始页
  /* 使用前先擦除 */
//  for(EraseCounter = 0; (EraseCounter < First_Page) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
	  //如果擦除失败，则进去死循环
    if (FLASH_ErasePage(FLASH_Start_Addr)!= FLASH_COMPLETE)//擦除以FLASH_Start_Addr为起始地址的整个flash页
    {
      while (1)
      {	
      }
    }
 }
	
  /* 写入FLASH */
  Address = FLASH_Start_Addr;
  while (len--)//将全部的半字都写进去
  {
    if (FLASH_ProgramHalfWord(Address, *data) == FLASH_COMPLETE)//如果将*data地址下的数据写入成功
    {
		data++;//写入下一个地址的数据
		Address = Address + 2;//因为写入的半字，所以地址要加2
    }
    else
    { 
      while (1)
      {
      }
    }
  }
  FLASH_Lock();/* 上锁 */
	
}	


/******************************************************************************
 * 函数名称: FLASH_read
 * 函数功能: 将flash存储的数据传导到数组里
 * 入口参数: *data 数组地址，len 长度
 * 返回值：
 * 备注: 
 ******************************************************************************/
void FLASH_read(int16_t *data,uint8_t len)
{
  Address = FLASH_Start_Addr;
  
  while (len--)//将全部数据读取出来
  {
    *data = *(__IO int16_t *)Address;//使用指针读指定地址下的存储器
		data++;//写入下一个地址的数据
    Address = Address + 2;//因为写入的半字，所以地址要加2
  }
}

