#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIM1_PWM_Init(u32 arr,u32 psc);
void Change_Speed(u32 ccr);
#endif
