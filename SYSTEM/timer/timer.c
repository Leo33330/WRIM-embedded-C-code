#include "timer.h"
#include "spi.h"
#include "spi1.h"

u16 time_ms;
u8 time_sec;
u8 time_min;
acc acc_top, acc_left, acc_right, acc_rear;
signed short rotation_x, rotation_y, rotation_z;
signed long rot_x_sum, rot_y_sum, rot_z_sum;
u16 storage_addr = 0;
u16 sector_num = 0;

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  


	TIM_Cmd(TIM3, ENABLE);  	 
}

void TIM3_IRQHandler(void)  //obtains sensor data, checks impact and writes data into flash memory if there is an impact
{
	u8 counter;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  ); 
		time_ms++;
		if(time_ms == 1000) // updates time
		{
		  time_sec++;
			time_ms = 0;
		}
		if(time_sec == 60)
		{
		  time_min++;
			time_sec = 0;
		}
		read_acc(&acc_top);
		read_acc(&acc_left);
		read_acc(&acc_right);
		read_acc(&acc_rear);
		for(counter = 0; counter < 10; counter++) // noisy gyro, take average of 10 at one instant
		{
		  read_rotation(&rotation_x, &rotation_y, &rotation_z);
			rot_x_sum+=rotation_x;
			rot_y_sum+=rotation_y;
			rot_z_sum+=rotation_z;
		}
		  rotation_x = rot_x_sum / 10;
		  rot_x_sum = 0;
		  rotation_y = rot_y_sum / 10;
		  rot_y_sum = 0;
		  rotation_z = rot_z_sum / 10;
		  rot_z_sum = 0;
		check_impact(&acc_top, &acc_left, &acc_right, &acc_rear, time_ms, time_sec, time_min, rotation_x, rotation_y, rotation_z);
		}
}
