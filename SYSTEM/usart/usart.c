#include "sys.h"
#include "usart.h"	  
#include "stdlib.h"
#include "string.h"
#include "delay.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif
 
#if 1
#pragma import(__use_no_semihosting)             

struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       

_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);  
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX    

u8 data[200];
u8 O_check = 0;
u8 K_check = 0;
u16 O_counter = 0;
u16 K_counter = 0;
char set_mode[] = "AT+CWMODE=1";
char restart_module[] = "AT+RST";
char connect_router[] = "AT+CWJAP=\"BoyNextDoor\",\"123456654321\""; //input router id and router password
char esp8266_ip[] = "AT+CIFSR";
char TCP_ip_config[] = "AT+CIPSTART=\"TCP\",\"192.168.1.2\",8080"; //input server's ip and port number
char serial_net_mode[] = "AT+CIPMODE=1";
char serial_net_start[] = "AT+CIPSEND";
u8 part_ok = 0;
u16 counter = 0;
  
void uart_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);
  

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);                

}

void WIFI_cmd(char *cmd) //input string data that will be sent be computer end
{
     		u8 i;
	      for(i=0;i<strlen(cmd);i++)
			{
				//USART_SendData(USART1, cmd[i]);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
				USART_SendData(USART1, cmd[i]);
			}
			  //USART_SendData(USART1, 0x0d);
			  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
				USART_SendData(USART1, 0x0d);
				//USART_SendData(USART1, 0x0a);
			  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
				USART_SendData(USART1, 0x0a);
}

void WIFI_init(void)
{
  WIFI_cmd(set_mode);
	while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;
	WIFI_cmd(restart_module);
	while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;
	delay_ms(500);
	WIFI_cmd(connect_router);
	while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;
	delay_ms(500);
  WIFI_cmd(esp8266_ip);
  while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;	
  WIFI_cmd(TCP_ip_config);
  while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;
	WIFI_cmd(serial_net_mode);
	while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;	
	WIFI_cmd(serial_net_start);
	while(!part_ok){}
	part_ok = 0, O_check = 0, K_check = 0, O_counter = 0, K_counter = 0, counter = 0;				
}

void USART1_IRQHandler(void) //used at WIFI init stage only for uart data receiving         
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
		{
		Res =USART_ReceiveData(USART1);
		data[counter] = Res;
    if(Res == 0x4f)
		{
		  O_check = 1;
			O_counter = counter;
			
		}
		if(Res == 0x4b)
		{
		  K_check = 1;
			K_counter = counter;
		}
		if(O_check && K_check && (K_counter == O_counter+1)) part_ok = 1;
    counter++;
 		 
     } 
#if SYSTEM_SUPPORT_OS 
	OSIntExit();  											 
#endif
} 
#endif	

