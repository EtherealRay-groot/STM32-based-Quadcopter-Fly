#ifndef __ALL_DEFINE_H__
#define __ALL_DEFINE_H__

#include "stm32f10x.h"                  // Device header
#include "ALL_DATA.h"

#include "IIC.h"
#include "SPI.h"
#include "ADC.h"
#include "TIM.h"
#include "USART.h"

#include "delay.h"
#include "Init.h"

#include "MPU6050.h"
#include "LED.h"
#include "NRF24L01.h"
#include "OLED.h"

#include "ANO_DT.h"
#include "Remote.h"
#include "control.h"
#include "myMath.h"
#include "USB_HID.h"

#include "imu.h"

#undef SUCCESS  //清空之前对SUCCESS的定义
#define SUCCESS 0
#undef FAILED
#define FAILED  1


#endif
