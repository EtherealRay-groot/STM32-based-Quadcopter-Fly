#include "ADC_DMA.h"
#include "stm32f10x.h"                  // Device header

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
__IO uint16_t ADC_ConvertedValue[4];


/******************************************************************************
 * 函数名称: ADC1_DMA_Config
 * 函数功能: 配置ADC1的DMA功能
 * 入口参数: 无
 * 返回值：无
 * 备注: 将GPIOA的PIN0、PIN1、PIN2、PIN3配置为模拟输入
		 将ADC1配置为不使用外部触发转换的独立连续扫描模式，一共四个通道，向右对齐
		 配置ADC1的时钟为6MHz
		 设置规则组皆为55.5个周期，序列
		 将DMA配置为
 ******************************************************************************/
void ADC1_DMA_Config(void)
{
	/*结构体初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);//开启GPIOA和ADC1时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//开启DMA1时钟
	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//设置为模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//配置PA0，PA1，PA2，PA3为模拟输入口
	/*ADC1初始化*/
	ADC_InitStructure.ADC_Mode =ADC_Mode_Independent;//设置为独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//启用扫描模式，用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//向右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;//一共有4个要转换的通道
	ADC_Init(ADC1, &ADC_InitStructure);//配置ADC1为4个通道向右对齐的独立模式连续扫描模式，不使用外部触发转换
	
	/*配置ADC时钟，为PCLK2的8分频，即48/8MHz =6MHz,ADC频率最高不能超过14MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	/*设置规则组的采样周期和序列*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	ADC_Cmd(ADC1, ENABLE);//使能ADC1
	
	/*DMA初始化*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	//外设基地址，给定形参AddrA
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = 4;//转运的数据大小（转运次数），与ADC通道数一致
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不自增，始终以ADC数据寄存器为源
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16为的ADC数据寄存器
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//模式，选择循环模式，与ADC的连续转换一致
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级，选择高等
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//将结构体变量交给DMA_Init，配置DMA1的通道1
	DMA_Cmd(DMA1_Channel1, ENABLE);							//DMA1的通道1使能
	
	ADC_DMACmd(ADC1, ENABLE);								//ADC1触发DMA1的信号使能
	
	/*ADC校准*/
	ADC_ResetCalibration(ADC1);/*复位校准寄存器 */ 
	while(ADC_GetResetCalibrationStatus(ADC1));/*等待校准寄存器复位完成 */
	ADC_StartCalibration(ADC1);/* ADC校准 */
	while(ADC_GetCalibrationStatus(ADC1));/* 等待校准完成*/
	
	/*ADC触发*/
	//软件触发ADC开始工作，由于ADC处于连续转换模式，故触发一次后ADC就可以一直连续不断地工作
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//由于没有采用外部触发，所以使用软件触发ADC转换
}



