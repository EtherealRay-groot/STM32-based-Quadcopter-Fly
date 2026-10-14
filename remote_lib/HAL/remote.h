#ifndef _REMOTE_H
#define _REMOTE_H


#include "sys.h"

typedef volatile struct 
{
	uint16_t roll;
	uint16_t pitch;
	uint16_t thr;
	uint16_t yaw;	
	uint16_t AUX1;
	uint16_t AUX2;
	uint16_t AUX3;
	uint16_t AUX4;
}_st_Remote;

extern _st_Remote Remote;

void RC_Init(void);
void NRF_Send(void);
void Key_Scan(void);
void RC_Analy(void);

#endif
