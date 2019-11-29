#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define EN_USART1_RX 			1		
	  	
extern u8 O_check;
extern u8 K_check;
extern u16 O_counter;
extern u16 K_counter;

void uart_init(u32 bound);
void WIFI_cmd(char *cmd);
void WIFI_init(void);
#endif

