#include "ALL_DATA.h"
#include "nrf24l01.h"
#include "control.h"
#include <math.h>
#include "myMath.h"
#include "LED.h"
#include "Remote.h"
#include "mpu6050.h"
#include "imu.h"

#define SUCCESS 0
#undef FAILED
#define FAILED  1


uint8_t RC_rxData[32];//将接收到的数据存在RC_rxData

/******************************************************************************
 * 函数名称: RC_Analy
 * 函数功能: 遥控数据解析
 * 入口参数: 无
 * 返回值：无
 * 备注: 匿名通信协议（四个字节分别为两个帧头，一个识别帧，一个有效数据长度（32-5是因为一次发送32个字节，其中5个字节不是数据））
			字节5和字节6是横滚的高字节和低字节（1000-2000）
			字节7和字节8是俯仰的高字节和低字节（1000-2000）
			字节9和字节10是油门的高字节和低字节（1000-2000）
			字节11和字节12是偏航的高字节和低字节（1000-2000）
			字节13和字节14是AUX1的高字节和低字节（1000或2000）
			字节15和字节16是AUX2的高字节和低字节（1000或2000）
			字节17和字节18是AUX3的高字节和低字节（1000或2000）
			字节19和字节20是AUX4的高字节和低字节（1000或2000）
			字节32是合校验位
 ******************************************************************************/
void RC_Analy(void)
{
	static uint16_t cnt;//超时标志位
	
	if(NRF24L01_RxPacket(RC_rxData)==SUCCESS)//遥控数据接收成功
	{
		uint8_t CheckSum=0;//校验和位
		cnt = 0;//超时归零
		
		for(uint8_t i=0;i<31;i++)
		{
			CheckSum += RC_rxData[i];//把前31位的数据全部加起来
		}
		if(RC_rxData[31] == CheckSum && RC_rxData[0] == 0xAA && RC_rxData[1] == 0xAF)//检查校验位和帧头是否正确
		{
			Remote.roll = ((uint16_t)RC_rxData[4]<<8) | RC_rxData[5];  //横滚（通道1），将两个uint8_t融合成一个uint16_t
			Remote.roll = LIMIT(Remote.roll,1000,2000);//限幅在1000-2000（从遥控获取的数据来看，横滚的大小在1020-2000）中值在1535左右

			Remote.pitch = ((uint16_t)RC_rxData[6]<<8) | RC_rxData[7];  //俯仰（通道2）
			Remote.pitch = LIMIT(Remote.pitch,1000,2000);//限幅在1000-2000（从遥控获取的数据来看，横滚的大小在1000-1995）中值在1484左右

			Remote.thr = ((uint16_t)RC_rxData[8]<<8) | RC_rxData[9];   //油门（通道3）	
			Remote.thr = LIMIT(Remote.thr,1000,2000);//限幅在1000-2000（从遥控获取的数据来看，油门的大小在1001-1817）

			Remote.yaw = ((uint16_t)RC_rxData[10]<<8) | RC_rxData[11];   //偏航（通道4）
			Remote.yaw = LIMIT(Remote.yaw,1000,2000);//限幅在1000-2000（从遥控获取的数据来看，偏航的大小在1000-2000）中值在1504左右

			Remote.AUX1 =  ((uint16_t)RC_rxData[12]<<8) | RC_rxData[13];   //AUX1（通道5）
			Remote.AUX1 =  LIMIT(Remote.AUX1,1000,2000);//限幅在1000-2000
			
			Remote.AUX2 =  ((uint16_t)RC_rxData[14]<<8) | RC_rxData[15];   //AUX2（通道6）
			Remote.AUX2 =  LIMIT(Remote.AUX2,1000,2000);//限幅在1000-2000
			
			Remote.AUX3 =  ((uint16_t)RC_rxData[16]<<8) | RC_rxData[17];   //AUX3（通道7）
			Remote.AUX3 =  LIMIT(Remote.AUX3,1000,2000);//限幅在1000-2000
			
			Remote.AUX4 =  ((uint16_t)RC_rxData[18]<<8) | RC_rxData[19];   //AUX4（通道8）
			Remote.AUX4 = LIMIT(Remote.AUX4,1000,2000);	//限幅在1000-2000
			
			//开始处理遥控数据
			{
				const float roll_pitch_ratio = 0.04f;//遥控控制姿态的量，+-500*0.04=+-20度
				const float yaw_ratio = 0.3f;		//代表控制偏航角的旋转速度
				
				pidPitch.desired =-(Remote.pitch-1500)*roll_pitch_ratio; //将遥杆值作为飞行角度的期望值（遥感输出1000为20度的俯仰角，2000为-20度的俯仰角）
				pidRoll.desired = -(Remote.roll-1500)*roll_pitch_ratio;//将遥杆值作为飞行角度的期望值（遥感输出1000为20度的横滚角，2000为-20度的横滚角）
				
				if(Remote.yaw>1820)//如果偏航摇杆输出大于了1820，每次偏航角的理想值减去0.3（顺时针转）
					{
						pidYaw.desired -= yaw_ratio;	
					}
				else if(Remote.yaw <1180)//如果偏航摇杆输出小于1180，每次偏航角的理想值加上0.3（逆时针转）
					{
						pidYaw.desired += yaw_ratio;	
					}						
			}
			Remote_unlock();//解锁判断
			
		}//END接收到正确数据


		else//如果没有接收到正确的数据
		{
			cnt++;
			if(cnt>500)//3ms进一次中断，500*3ms=1.5s,1.5秒没有接收到数据自动关机
			{	
				cnt = 0;//归零
				ALL_flag.unlock = 0;//置锁定 
				NRF24L01_Init();//重启NRF24L01
			}
			
		}//END接收到错误数据
		
	}//END接收到数据
	
}

/******************************************************************************
 * 函数名称: Remote_unlock
 * 函数功能: 解锁判断
 * 入口参数: 无
 * 返回值：无
 * 备注: 解锁三步：油门低，油门高，油门低，LED不闪了就解锁了
 ******************************************************************************/
void Remote_unlock(void)
{
	volatile static uint8_t status = WAITING_1;//初始状态为等待解锁
	static uint16_t cnt=0;//记录超时时间
	
	if(Remote.thr<1050 &&Remote.yaw<1200)//如果油门遥杆置左下角，则锁定
	{
		status = EXIT_255;//进入锁定模式
	}
	
	switch(status)
	{
		case WAITING_1://等待解锁1
		{
			if(Remote.thr<1150)//油门小于1150
			{
				status = WAITING_2;//进入下一状态
			}
			break;
		}/*  END  */
		case WAITING_2://等待解锁2
		{
			if(Remote.thr>1600)//油门大于1600
			{
				static uint8_t cnt = 0;
				cnt++;		
				if(cnt>5) //最高油门需保持200ms以上，防止遥控开机初始化未完成产生错误数据
				{	
					cnt=0;
					status = WAITING_3;//进入下一阶段
				}				
			}
			break;
		}/*  END  */
		case WAITING_3://等待解锁3
		{
			if(Remote.thr<1150)//油门小于1150          
			{			 
				status = WAITING_4;//进入下一阶段			 
			}
			break;
		}/*  END  */
		case WAITING_4:	//解锁前准备
		{
			ALL_flag.unlock = 1;//设置为解锁模式
			status = PROCESS_31;//进入下一阶段
			LED.status = AlwaysOn;//将LED的灯设为常亮	
			imu_rest();	//初始化四元数	
			break;		
		}/*  END  */
		case PROCESS_31:	//进入解锁状态
		{
			if(Remote.thr<1020)//油门处于很低的位置，9s自动上锁
			{
				if(cnt++ > 3000)                                     // 油门遥杆处于最低9S自动上锁
				{								
					status = EXIT_255;//进入上锁模式								
				}
			}
			else if(!ALL_flag.unlock)//因为其他原因进入锁定
			{
				status = EXIT_255;		
			}
			else //如果不是以上两种情况，不进入cnt计数
			{
				cnt = 0;
			}
			break;
		}/*  END  */
		case EXIT_255://锁定前准备
		{
			LED.status = AllFlashLight;//LED闪烁
			cnt = 0;//计数清零
			LED.FlashTime = 100; //100ms		
			ALL_flag.unlock = 0;//进入锁定模式
			status = WAITING_1;//进入锁定模式
			break;
		}/*  END  */
		default://以上case都不满足
		{
			status = EXIT_255;//锁定
			break;
		}/*  END  */
	}//END SWITCH
}//END FUNCTION


