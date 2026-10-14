#ifndef __PID_H
#define __PID_H
#include "ALL_DATA.h"
#include <stdbool.h>

void CascadePID(PidObject* pidRate,PidObject* pidAngE,const float dt);  //串级PID

void pidRest(PidObject **pid,const uint8_t len); //pid数据复位

void pidUpdate(PidObject* pid,const float dt);  //PID数据更新

#endif

