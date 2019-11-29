#include "delay.h"
#include "sys.h"
#include "usart.h"	
#include "stm32f10x.h"
#include "spi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "timer.h"
#include "spi1.h"
 
u8 H3LIS_ID, mpu9250_id, impacts_sent; //declare accelerometer and gyro id check variables, declare number of impacts detected so far
char text_buf[100]; //reserve string space for WIFI transmission
u8 SPI_read_buf[1]; //buff u8 for reading data from W25Q flash
float decimal_part;
int data_int_part, data_decimal_part, time_sec_int, time_ms_int, time_min_int; //variables for data convension from flash memory to ASCII friendly
signed short read_data_buf; //actual impact data in flash (hex numbers)
u32 reading_address; //current impact data reading address in flash meomry

int main(void)
 {
   u8 counter; //declare a counter to set up multiple accelerometers
   
	 delay_init(); // enable delay functions
	 uart_init(115200); //set up WIFI to uart baud rate
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	 SPI_bus_Init(); //spi2 init (sensors)
	 SPI_Flash_Init(); //spi1 init (W25Q flash memory)
   
	 mpu9250_id = mpu9250_read_whoim(); //read gyro id
	 
	 WIFI_init(); //WIFI init
	 
	 for(counter = 1; counter < 100; counter++) // reserve 100 sectors on W25Q for impacts
	 {
	   SPI_Flash_Erase_Sector(counter);
	 }
	 
  	 H3LIS_ID = H3LIS_Read_WhoIam(1); // read accelerometers' ids
	   H3LIS_ID = H3LIS_Read_WhoIam(2);
	   H3LIS_ID = H3LIS_Read_WhoIam(3);
	   H3LIS_ID = H3LIS_Read_WhoIam(4);
	 
	 H3LIS_set_id(&acc_top, 1); // top accelerometer (numbered as 1)
	 H3LIS_set_id(&acc_left, 2); // top accelerometer (numbered as 2)
	 H3LIS_set_id(&acc_right, 3); // top accelerometer (numbered as 3)
	 H3LIS_set_id(&acc_rear, 4); // top accelerometer (numbered as 4)
	 
	 for(counter = 1; counter < 5; counter++) //set accelerometer properties (check datasheet for register details)
   H3LIS_setpower_outputfrequency_axis(counter);
	 
	 for(counter = 1; counter < 5; counter++) //set max acceleration to be 200g
   H3LIS_setscale(counter);
	 
	 mpu9250_config(); //set gyro properties (check datasheet for register details)
	 mpu9250_gyro_config(); //set max rotation speed to be 1000 deg/s
	 WIFI_cmd("Initialization Completed!"); //transmit this to computer end and indicates that initialization is completed
	 TIM3_Int_Init(9,7199); // 1ms timer interrupt to obtain sensor readings, to compare to threshold values, and to save impact data if it happens
	 
	 while(1)
	 {
		 if(impacts_sent < impact_num) //transmit impact data if not all impact data has been transmitted (30 data points each time)
		 {
			 for(counter = 0; counter < 30; counter++)
			 {
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34, 1);
				 time_min_int = SPI_read_buf[0];
				 SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 1, 1);
				 time_sec_int = SPI_read_buf[0];
				 SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 2, 1);
				 time_ms_int = SPI_read_buf[0];
				 SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 3, 1);
				 time_ms_int = (time_ms_int << 8) + SPI_read_buf[0];
				 sprintf(text_buf,"%d.%d.%d",time_min_int,time_sec_int,time_ms_int);
				 WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 4, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 5, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 6, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 7, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);    
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 8, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 9, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf); 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 10, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 11, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);   	
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 12, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 13, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);						 
         WIFI_cmd(text_buf);  				 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 14, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 15, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 16, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 17, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);  
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 18, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 19, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);  
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 20, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 21, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 22, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 23, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 24, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 25, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 26, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 27, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf*98.0/1000;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 28, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 29, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf/32.8;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 30, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 31, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf/32.8;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 32, 1);
				 read_data_buf = SPI_read_buf[0];			 
			   SPI_Flash_Read(SPI_read_buf,impacts_sent*4096 + counter*34 + 33, 1);
				 read_data_buf = (read_data_buf << 8) + SPI_read_buf[0];		
         decimal_part = read_data_buf/32.8;
         data_int_part = decimal_part;
         decimal_part -= data_int_part;
         if(decimal_part < 0)
         data_decimal_part = -decimal_part*1000;
         else
         data_decimal_part = decimal_part*1000;				
         sprintf(text_buf,"%d.%d", data_int_part, data_decimal_part);			
         WIFI_cmd(text_buf);
         WIFI_cmd("");				 
			 }
			 impacts_sent++;
		 }		

	 }
}
 
