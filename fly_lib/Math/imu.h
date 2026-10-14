#ifndef __IMU_H
#define	__IMU_H

#include "ALL_DATA.h"

void GetAngle(const _st_Mpu *pMpu,_st_AngE *pAngE, float dt);
void imu_rest(void);

#endif
