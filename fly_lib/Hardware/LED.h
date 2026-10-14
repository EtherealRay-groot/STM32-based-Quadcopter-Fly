#ifndef __LED_H
#define __LED_H

#include "stdint.h"

////”“«∞µ∆			 
#define Right_Front_LED1_H()  		TIM3->CCR1=1000 //∞µ£®…Ë÷√≤∂ªÒ±»Ωœ÷µ£©
#define Right_Front_LED1_L()  		TIM3->CCR1=500  //¡¡
#define Right_Front_LED1_Toggle()   TIM3->CCR1^=(1000^500)//…¡À∏

////◊Û«∞µ∆			 
#define Left_Front_LED3_H() 		TIM3->CCR3=1000 //∞µ
#define Left_Front_LED3_L()  		TIM3->CCR3=500  //¡¡
#define Left_Front_LED3_Toggle()  	TIM3->CCR3^=(1000^500)//…¡À∏
//-------------------------------------------------
////”“∫Ûµ∆			 
#define Rigth_Black_LED2_H()  		TIM3->CCR2=1000 //∞µ
#define Rigth_Black_bLED2_L()  		TIM3->CCR2=500  //¡¡
#define Rigth_Black_LED2_Toggle()   TIM3->CCR2^=(1000^500)//…¡À∏
////◊Û∫Ûµ∆			 
#define Left_Black_LED4_H()  		TIM3->CCR4=1000 //∞µ
#define Left_Black_LED4_L()  		TIM3->CCR4=500  //¡¡
#define Left_Black_LED4_Toggle()  	TIM3->CCR4^=(1000^500)//…¡À∏



typedef struct
{
	uint16_t FlashTime;
	enum
	{
		AlwaysOn, //≥£¡¡
		AlwaysOff, //≥£√
		AllFlashLight,//“ª÷±…¡À∏
		AlternateFlash, 
		WARNING,
		DANGEROURS,
		GET_OFFSET	
	}status; 
}sLED;	

extern sLED LED;
void Pilot_LED(void);

#endif 
