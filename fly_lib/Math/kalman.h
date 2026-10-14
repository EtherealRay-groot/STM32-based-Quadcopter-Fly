#ifndef _KALMAN_H
#define _KALMAN_H

struct _1_ekf_filter
{
	float LastP;
	float Now_P;
	float out;
	float Kg;//卡尔曼增益
	float Q;//系统过程的协方差
	float R;//测量的协方差
};

void kalman_1(struct _1_ekf_filter *ekf,float input);  //一维卡尔曼

#endif

