#include "ADC.h"
#include "stm32f10x.h"                  // Device header

uint16_t ADC_Value[10];
  /* Configure PC.0-5 (ADC Channel10-15) as analog input -------------------------*/
  
  
  
/******************************************************************************
 * 函数名称: ADC_DMA_Init
 * 函数功能: ADC、DMA、GPIO初始化配置
 * 入口参数: 无
 * 返回值：无
 * 备注: 无
 ******************************************************************************/
void ADC_DMA_Init(void)
{
  //定义结构体名
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//开启DMA1的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);//开启ADC1的时钟，开启GPIOB的时钟

//**********************************GPIO配置*************************************//
  //设置PB0为模拟输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//**********************************DMA1通道1配置*************************************//
  DMA_DeInit(DMA1_Channel1);//反初始化
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//外设基地址：ADC1的DR数据寄存器
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Value;//存储器基地址：给定存放AD转换结果的全局数组AD_Value
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;		 //数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
  DMA_InitStructure.DMA_BufferSize = 10;					 //转运的数据大小（转运次数），与ADC通道数一致
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址自增，选择失能，始终以ADC数据寄存器为源
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//存储器地址自增，选择使能，每次转运后，数组移到下一个位置
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16为的ADC数据寄存器
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//存储器数据宽度，选择半字，与源数据宽度对应
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//模式，选择循环模式，与ADC的连续转换一致
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//优先级，选择高等 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);//将结构体变量交给DMA_Init，配置DMA1的通道1

  DMA_Cmd(DMA1_Channel1, ENABLE);//DMA1的通道1使能
  
  
//**********************************ADC1配置*************************************//
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//选择独立模式，即单独使用ADC1
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式，使能，扫描规则组的序列，扫描数量由ADC_NbrOfChannel确定 
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换，使能，每转换一次规则组序列后立刻开始下一次转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发，使用软件触发，不需要外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐，选择右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;//通道数，为1，扫描规则组的前1个通道
  
  ADC_Init(ADC1, &ADC_InitStructure);//将结构体变量交给ADC_Init，配置ADC1

  /* ADC1 regular channel configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);//规则组。ADC1通道8,1个成员，转换时间55.5个ADC周期

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);//ADC1触发DMA1的信号使能
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);//ADC1使能

//固定流程，内部有电路会自动执行校准 
  ADC_ResetCalibration(ADC1);//复位校准
  while(ADC_GetResetCalibrationStatus(ADC1));//等待复位校准完成
  ADC_StartCalibration(ADC1);//开始校准
  while(ADC_GetCalibrationStatus(ADC1));//等待校准完成
  
  /*ADC触发*/ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件触发ADC开始工作，由于ADC处于连续转换模式，故触发一次后ADC就可以一直连续不断地工作
}





uint16_t GetADCValue(void)
{
 uint8_t i;
 uint32_t tmp;
 for(i=0;i<10;i++)
	tmp +=ADC_Value[i];
 return (tmp/10);
}
