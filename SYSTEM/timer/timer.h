#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 
#include "spi.h"

extern acc acc_top, acc_left, acc_right, acc_rear;
extern u8 impact_flag;

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_IRQHandler(void);

#endif
