#include "spi.h"
#include "delay.h"
#include "usart.h"

void SPI_bus_Init(void)
{	
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_10);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
  H3LIS_CS_top=1;				
	H3LIS_CS_left=1;
	H3LIS_CS_right=1;
	H3LIS_CS_rear=1;
	MPU9250_CS=1;
	SPI2_Init();		   
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);
}  

void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(SPI2, &SPI_InitStructure); 
 
	SPI_Cmd(SPI2, ENABLE);
	
	SPI2_ReadWriteByte(0xff);	 
 
}

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;
	SPI_Cmd(SPI2,ENABLE); 

}

u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) 
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData);
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) 
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); 				    
}

u8 H3LIS_Read_WhoIam(u8 id) //check accelerometer's id, returns 0x32 if correct  
{    										    
	u8 temp = 0;
	if(id == 1)
	{
	H3LIS_CS_top=0;                            
    SPI2_ReadWriteByte(0x8f);         	   
    temp = SPI2_ReadWriteByte(0Xff);
	H3LIS_CS_top=1;
	}  
	else if(id == 2)
	{
	H3LIS_CS_left=0;                            
    SPI2_ReadWriteByte(0x8f);         	   
    temp = SPI2_ReadWriteByte(0Xff);
	H3LIS_CS_left=1;
	}	
	else if(id == 3)
	{
	H3LIS_CS_right=0;                            
    SPI2_ReadWriteByte(0x8f);         	   
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;
	}
	else if(id == 4)
	{
	H3LIS_CS_rear=0;                            	 
    SPI2_ReadWriteByte(0x8f);         	    
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_rear=1;
	}
return temp;	
}  

void H3LIS_setpower_outputfrequency_axis(u8 id)   
{    										    
	if(id == 1) 
		{
			H3LIS_CS_top=0;                            	
      SPI2_ReadWriteByte(0x20);         	   
      SPI2_ReadWriteByte(0X3f);   	
	    H3LIS_CS_top=1;
		}  	
	else if(id == 2) 
		{
			H3LIS_CS_left=0;                            	   
      SPI2_ReadWriteByte(0x20);            
      SPI2_ReadWriteByte(0X3f);   	
	    H3LIS_CS_left=1;
		}
	else if(id == 3) 
		{
			H3LIS_CS_right=0;                            
      SPI2_ReadWriteByte(0x20);         	    
      SPI2_ReadWriteByte(0X3f);   	
	    H3LIS_CS_right=1;
		}
	else if(id == 4) 
		{
			H3LIS_CS_rear=0;                            	
      SPI2_ReadWriteByte(0x20);         	 
      SPI2_ReadWriteByte(0X3f);   	
	    H3LIS_CS_rear=1;
		}		
}  

void H3LIS_setscale(u8 id)   
{    										    
	if(id == 1) 
		{
			H3LIS_CS_top=0;                            
      SPI2_ReadWriteByte(0x23);             
      SPI2_ReadWriteByte(0X90);   	
	    H3LIS_CS_top=1;
		}  
	else if(id == 2) 
		{
			H3LIS_CS_left=0;                            	  
      SPI2_ReadWriteByte(0x23);         	    
      SPI2_ReadWriteByte(0X90);   	 
	    H3LIS_CS_left=1;
		} 	
	else if(id == 3) 
		{
			H3LIS_CS_right=0;                            	 
      SPI2_ReadWriteByte(0x23);         	   
      SPI2_ReadWriteByte(0X90);   	 
	    H3LIS_CS_right=1;
		} 
	else if(id == 4) 
		{
			H3LIS_CS_rear=0;                            	
      SPI2_ReadWriteByte(0x23);            
      SPI2_ReadWriteByte(0X90);   	
	    H3LIS_CS_rear=1;
		} 
} 


void read_acc(acc *a) //obtains accelerometer readings from output registers (based on accelerometer's struct id)
{  	
	if(a->acc_id == 1) {H3LIS_CS_top=0;                            
    a->acc_z_hex = read_z_high(1);    
    a->acc_z_hex = (a->acc_z_hex<<8) + read_z_low(1);     
    a->acc_x_hex = read_x_high(1);    
    a->acc_x_hex = (a->acc_x_hex<<8) + read_x_low(1);    
    a->acc_y_hex = read_y_high(1);    
    a->acc_y_hex = (a->acc_y_hex<<8) + read_y_low(1);   
		H3LIS_CS_top=1;
	  a->acc_z_hex = a->acc_z_hex >> 4;
	  a->acc_x_hex = a->acc_x_hex >> 4;
	  a->acc_y_hex = a->acc_y_hex >> 4;
	  } 
	else if(a->acc_id == 2) {H3LIS_CS_left=0;                            
    a->acc_z_hex = read_z_high(2);    
    a->acc_z_hex = (a->acc_z_hex<<8) + read_z_low(2);     
    a->acc_x_hex = read_x_high(2);    
    a->acc_x_hex = (a->acc_x_hex<<8) + read_x_low(2);   	    
    a->acc_y_hex = read_y_high(2);    
    a->acc_y_hex = (a->acc_y_hex<<8) + read_y_low(2);   	
		H3LIS_CS_left=1;
	  a->acc_z_hex = a->acc_z_hex >> 4;
	  a->acc_x_hex = a->acc_x_hex >> 4;
	  a->acc_y_hex = a->acc_y_hex >> 4;
	  }
	else if(a->acc_id == 3) {H3LIS_CS_right=0;                            	 
    a->acc_z_hex = read_z_high(3);    
    a->acc_z_hex = (a->acc_z_hex<<8) + read_z_low(3);     
    a->acc_x_hex = read_x_high(3);    
    a->acc_x_hex = (a->acc_x_hex<<8) + read_x_low(3);      
    a->acc_y_hex = read_y_high(3);    
    a->acc_y_hex = (a->acc_y_hex<<8) + read_y_low(3);   	
		H3LIS_CS_right=1;
	  a->acc_z_hex = a->acc_z_hex >> 4;
	  a->acc_x_hex = a->acc_x_hex >> 4;
	  a->acc_y_hex = a->acc_y_hex >> 4;
	  }
	else if(a->acc_id == 4) {H3LIS_CS_rear=0;                            	
    a->acc_z_hex = read_z_high(4);    
    a->acc_z_hex = (a->acc_z_hex<<8) + read_z_low(4);     
    a->acc_x_hex = read_x_high(4);    
    a->acc_x_hex = (a->acc_x_hex<<8) + read_x_low(4);       
    a->acc_y_hex = read_y_high(4);    
    a->acc_y_hex = (a->acc_y_hex<<8) + read_y_low(4);   	
		H3LIS_CS_rear=1;
	  a->acc_z_hex = a->acc_z_hex >> 4;
	  a->acc_x_hex = a->acc_x_hex >> 4;
	  a->acc_y_hex = a->acc_y_hex >> 4;
	  }
}

u8 read_z_high(u8 id) //read high byte register of z axis of accelerometer  
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                            	
    SPI2_ReadWriteByte(0xad);         	   
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_top=1;} 
	else if(id == 2) {H3LIS_CS_left=0;                            	
    SPI2_ReadWriteByte(0xad);           
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_left=1;} 
	else if(id == 3) {H3LIS_CS_right=0;                            	
    SPI2_ReadWriteByte(0xad);         	   
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;} 
	else if(id == 4) {H3LIS_CS_rear=0;                            	
    SPI2_ReadWriteByte(0xad);         	   
    temp = SPI2_ReadWriteByte(0Xff);  
	H3LIS_CS_rear=1;} 
	return temp;
}

u8 read_z_low(u8 id)   //read low byte register of z axis of accelerometer
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                            	
    SPI2_ReadWriteByte(0xac);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_top=1;} 
	else if(id == 2) {H3LIS_CS_left=0;                            	
    SPI2_ReadWriteByte(0xac);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_left=1;} 
	else if(id == 3) {H3LIS_CS_right=0;                            	
    SPI2_ReadWriteByte(0xac);         	
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;} 
	else if(id == 4) {H3LIS_CS_rear=0;                            	
    SPI2_ReadWriteByte(0xac);         	  
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_rear=1;} 
	return temp;
}

u8 read_x_low(u8 id)   //read low byte register of x axis of accelerometer
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                            	
    SPI2_ReadWriteByte(0xa8);         
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_top=1;} 
	else if(id == 2) {H3LIS_CS_left=0;                            	
    SPI2_ReadWriteByte(0xa8);         	
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_left=1;}  
	else if(id == 3) {H3LIS_CS_right=0;                            	  
    SPI2_ReadWriteByte(0xa8);         	 
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;} 
	else if(id == 4) {H3LIS_CS_rear=0;                             
    SPI2_ReadWriteByte(0xa8);           
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_rear=1;} 	
	return temp;
}

u8 read_x_high(u8 id)   //read high byte register of x axis of accelerometer
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                            
    SPI2_ReadWriteByte(0xa9);         	 
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_top=1;} 	
	else if(id == 2) {H3LIS_CS_left=0;                             
    SPI2_ReadWriteByte(0xa9);         	
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_left=1;} 
	else if(id == 3) {H3LIS_CS_right=0;                           
    SPI2_ReadWriteByte(0xa9);         	  
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;} 
	else if(id == 4) {H3LIS_CS_rear=0;                            
    SPI2_ReadWriteByte(0xa9);         	   
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_rear=1;} 	
	return temp;
}

u8 read_y_low(u8 id)  //read low byte register of y axis of accelerometer 
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                            
    SPI2_ReadWriteByte(0xaa);          
    temp = SPI2_ReadWriteByte(0Xff);    
	H3LIS_CS_top=1;}  
	else if(id == 2) {H3LIS_CS_left=0;                           
    SPI2_ReadWriteByte(0xaa);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_left=2;} 
	else if(id == 3) {H3LIS_CS_right=0;                            	
    SPI2_ReadWriteByte(0xaa);         	    
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=2;} 
	else if(id == 4) {H3LIS_CS_rear=0;                            
    SPI2_ReadWriteByte(0xaa);         	  
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_rear=2;} 	
	return temp;
}

u8 read_y_high(u8 id)   //read high byte register of y axis of accelerometer
{ 
  u8 temp = 0;	
	if(id == 1) {H3LIS_CS_top=0;                              
    SPI2_ReadWriteByte(0xab);         	   
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_top=1;}
	else if(id == 2) {H3LIS_CS_left=0;                            	
    SPI2_ReadWriteByte(0xab);         	  
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_left=1;}
	else if(id == 3) {H3LIS_CS_right=0;                          
    SPI2_ReadWriteByte(0xab);         	
    temp = SPI2_ReadWriteByte(0Xff);   	
	H3LIS_CS_right=1;}
	else if(id == 4) {H3LIS_CS_rear=0;                             
    SPI2_ReadWriteByte(0xab);            
    temp = SPI2_ReadWriteByte(0Xff);   
	H3LIS_CS_rear=1;}	
	return temp;
}

u8 mpu9250_read_whoim() //check gyro id, returns 0x71 if correct
{    										     	
  u8 temp;
	MPU9250_CS=0;                          
    SPI2_ReadWriteByte(0xf5);            
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

void mpu9250_gyro_config()   
{    										     	
  MPU9250_CS=0;                            
    SPI2_ReadWriteByte(0x1b);         	   
    SPI2_ReadWriteByte(0X10);   	
	MPU9250_CS=1; 
} 

void mpu9250_config()   
{    										     	
  MPU9250_CS=0;                           
    SPI2_ReadWriteByte(0x1a);         	   
    SPI2_ReadWriteByte(0X01);   	
	MPU9250_CS=1; 
} 

u8 mpu9250_read_x_high()   //read high byte register of x axis of gyro
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc3);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

u8 mpu9250_read_x_low()   //read low byte register of x axis of gyro
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc4);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

u8 mpu9250_read_y_high()   //read high byte register of y axis of gyro
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc5);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

u8 mpu9250_read_y_low()  //read low byte register of y axis of gyro 
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc6);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

u8 mpu9250_read_z_high()  //read high byte register of z axis of gyro 
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc7);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
}

u8 mpu9250_read_z_low()   //read low byte register of z axis of gyro
{    										     	
  u8 temp = 0;
	MPU9250_CS=0;                            	
    SPI2_ReadWriteByte(0xc8);          
    temp = SPI2_ReadWriteByte(0Xff);   	
	MPU9250_CS=1; 
	return temp;
} 

void H3LIS_set_id(acc *a, u8 id)
{
  a->acc_id = id;
}

u32 flash_address;
u8 data_buf[34];
u32 current_address;
u8 impact_flag, impact_num, counter_30ms;

/*
check currently obtained accelerometers' data
if any value higher than threshold, record the following 30ms sensor data into one impact data sector
*/

void check_impact(acc *a, acc *b, acc *c, acc *d, u16 time_ms, u8 time_sec, u8 time_min, signed short rotation_x, signed short rotation_y, signed short rotation_z)
{
	if(impact_flag)
	{
		data_buf[0] = time_min;
		data_buf[1] = time_sec;
		data_buf[2] = time_ms >> 8;
		data_buf[3] = time_ms;
		data_buf[4] = a->acc_x_hex >> 8;
		data_buf[5] = a->acc_x_hex;
		data_buf[6] = a->acc_y_hex >> 8;
		data_buf[7] = a->acc_y_hex;
		data_buf[8] = a->acc_z_hex >> 8;
		data_buf[9] = a->acc_z_hex;
		data_buf[10] = b->acc_x_hex >> 8;
		data_buf[11] = b->acc_x_hex;
		data_buf[12] = b->acc_y_hex >> 8;
		data_buf[13] = b->acc_y_hex;
		data_buf[14] = b->acc_z_hex >> 8;
		data_buf[15] = b->acc_z_hex;
		data_buf[16] = c->acc_x_hex >> 8;
		data_buf[17] = c->acc_x_hex;
		data_buf[18] = c->acc_y_hex >> 8;
		data_buf[19] = c->acc_y_hex;
		data_buf[20] = c->acc_z_hex >> 8;
		data_buf[21] = c->acc_z_hex;
		data_buf[22] = d->acc_x_hex >> 8;
		data_buf[23] = d->acc_x_hex;
		data_buf[24] = d->acc_y_hex >> 8;
		data_buf[25] = d->acc_y_hex;
		data_buf[26] = d->acc_z_hex >> 8;
		data_buf[27] = d->acc_z_hex;
		data_buf[28] = rotation_x >> 8;
		data_buf[29] = rotation_x;
		data_buf[30] = rotation_y >> 8;
		data_buf[31] = rotation_y;
		data_buf[32] = rotation_z >> 8;
		data_buf[33] = rotation_z;
		SPI_Flash_Write((u8*)data_buf,current_address,34);
		current_address += 34;
    counter_30ms++;
    if(counter_30ms == 29)
		{
		  impact_flag = 0;
			impact_num++;
			flash_address += 4096;
      counter_30ms = 0;			
		}			
	}
	else if(a->acc_x_hex > 400 || a->acc_y_hex > 400 || a->acc_z_hex > 400 || a->acc_x_hex < -400 || a->acc_y_hex < -400 || a->acc_z_hex < -400)
	{
    current_address = flash_address;
		data_buf[0] = time_min;
		data_buf[1] = time_sec;
		data_buf[2] = time_ms >> 8;
		data_buf[3] = time_ms;
		data_buf[4] = a->acc_x_hex >> 8;
		data_buf[5] = a->acc_x_hex;
		data_buf[6] = a->acc_y_hex >> 8;
		data_buf[7] = a->acc_y_hex;
		data_buf[8] = a->acc_z_hex >> 8;
		data_buf[9] = a->acc_z_hex;
		data_buf[10] = b->acc_x_hex >> 8;
		data_buf[11] = b->acc_x_hex;
		data_buf[12] = b->acc_y_hex >> 8;
		data_buf[13] = b->acc_y_hex;
		data_buf[14] = b->acc_z_hex >> 8;
		data_buf[15] = b->acc_z_hex;
		data_buf[16] = c->acc_x_hex >> 8;
		data_buf[17] = c->acc_x_hex;
		data_buf[18] = c->acc_y_hex >> 8;
		data_buf[19] = c->acc_y_hex;
		data_buf[20] = c->acc_z_hex >> 8;
		data_buf[21] = c->acc_z_hex;
		data_buf[22] = d->acc_x_hex >> 8;
		data_buf[23] = d->acc_x_hex;
		data_buf[24] = d->acc_y_hex >> 8;
		data_buf[25] = d->acc_y_hex;
		data_buf[26] = d->acc_z_hex >> 8;
		data_buf[27] = d->acc_z_hex;
		data_buf[28] = rotation_x >> 8;
		data_buf[29] = rotation_x;
		data_buf[30] = rotation_y >> 8;
		data_buf[31] = rotation_y;
		data_buf[32] = rotation_z >> 8;
		data_buf[33] = rotation_z;
		SPI_Flash_Write((u8*)data_buf,current_address,34);
		current_address += 34;
		impact_flag = 1;
	}
	else if(b->acc_x_hex > 400 || b->acc_y_hex > 400 || b->acc_z_hex > 400 || b->acc_x_hex < -400 || b->acc_y_hex < -400 || b->acc_z_hex < -400)
	{
    current_address = flash_address;
		data_buf[0] = time_min;
		data_buf[1] = time_sec;
		data_buf[2] = time_ms >> 8;
		data_buf[3] = time_ms;
		data_buf[4] = a->acc_x_hex >> 8;
		data_buf[5] = a->acc_x_hex;
		data_buf[6] = a->acc_y_hex >> 8;
		data_buf[7] = a->acc_y_hex;
		data_buf[8] = a->acc_z_hex >> 8;
		data_buf[9] = a->acc_z_hex;
		data_buf[10] = b->acc_x_hex >> 8;
		data_buf[11] = b->acc_x_hex;
		data_buf[12] = b->acc_y_hex >> 8;
		data_buf[13] = b->acc_y_hex;
		data_buf[14] = b->acc_z_hex >> 8;
		data_buf[15] = b->acc_z_hex;
		data_buf[16] = c->acc_x_hex >> 8;
		data_buf[17] = c->acc_x_hex;
		data_buf[18] = c->acc_y_hex >> 8;
		data_buf[19] = c->acc_y_hex;
		data_buf[20] = c->acc_z_hex >> 8;
		data_buf[21] = c->acc_z_hex;
		data_buf[22] = d->acc_x_hex >> 8;
		data_buf[23] = d->acc_x_hex;
		data_buf[24] = d->acc_y_hex >> 8;
		data_buf[25] = d->acc_y_hex;
		data_buf[26] = d->acc_z_hex >> 8;
		data_buf[27] = d->acc_z_hex;
		data_buf[28] = rotation_x >> 8;
		data_buf[29] = rotation_x;
		data_buf[30] = rotation_y >> 8;
		data_buf[31] = rotation_y;
		data_buf[32] = rotation_z >> 8;
		data_buf[33] = rotation_z;
		SPI_Flash_Write((u8*)data_buf,current_address,34);
		current_address += 34;
		impact_flag = 1;
	}
	else if(c->acc_x_hex > 400 || c->acc_y_hex > 400 || c->acc_z_hex > 400 || c->acc_x_hex < -400 || c->acc_y_hex < -400 || c->acc_z_hex < -400)
	{
    current_address = flash_address;
		data_buf[0] = time_min;
		data_buf[1] = time_sec;
		data_buf[2] = time_ms >> 8;
		data_buf[3] = time_ms;
		data_buf[4] = a->acc_x_hex >> 8;
		data_buf[5] = a->acc_x_hex;
		data_buf[6] = a->acc_y_hex >> 8;
		data_buf[7] = a->acc_y_hex;
		data_buf[8] = a->acc_z_hex >> 8;
		data_buf[9] = a->acc_z_hex;
		data_buf[10] = b->acc_x_hex >> 8;
		data_buf[11] = b->acc_x_hex;
		data_buf[12] = b->acc_y_hex >> 8;
		data_buf[13] = b->acc_y_hex;
		data_buf[14] = b->acc_z_hex >> 8;
		data_buf[15] = b->acc_z_hex;
		data_buf[16] = c->acc_x_hex >> 8;
		data_buf[17] = c->acc_x_hex;
		data_buf[18] = c->acc_y_hex >> 8;
		data_buf[19] = c->acc_y_hex;
		data_buf[20] = c->acc_z_hex >> 8;
		data_buf[21] = c->acc_z_hex;
		data_buf[22] = d->acc_x_hex >> 8;
		data_buf[23] = d->acc_x_hex;
		data_buf[24] = d->acc_y_hex >> 8;
		data_buf[25] = d->acc_y_hex;
		data_buf[26] = d->acc_z_hex >> 8;
		data_buf[27] = d->acc_z_hex;
		data_buf[28] = rotation_x >> 8;
		data_buf[29] = rotation_x;
		data_buf[30] = rotation_y >> 8;
		data_buf[31] = rotation_y;
		data_buf[32] = rotation_z >> 8;
		data_buf[33] = rotation_z;
		SPI_Flash_Write((u8*)data_buf,current_address,34);
		current_address += 34;
		impact_flag = 1;
	}
	else if(d->acc_x_hex > 400 || d->acc_y_hex > 400 || d->acc_z_hex > 400 || d->acc_x_hex < -400 || d->acc_y_hex < -400 || d->acc_z_hex < -400)
	{
    current_address = flash_address;
		data_buf[0] = time_min;
		data_buf[1] = time_sec;
		data_buf[2] = time_ms >> 8;
		data_buf[3] = time_ms;
		data_buf[4] = a->acc_x_hex >> 8;
		data_buf[5] = a->acc_x_hex;
		data_buf[6] = a->acc_y_hex >> 8;
		data_buf[7] = a->acc_y_hex;
		data_buf[8] = a->acc_z_hex >> 8;
		data_buf[9] = a->acc_z_hex;
		data_buf[10] = b->acc_x_hex >> 8;
		data_buf[11] = b->acc_x_hex;
		data_buf[12] = b->acc_y_hex >> 8;
		data_buf[13] = b->acc_y_hex;
		data_buf[14] = b->acc_z_hex >> 8;
		data_buf[15] = b->acc_z_hex;
		data_buf[16] = c->acc_x_hex >> 8;
		data_buf[17] = c->acc_x_hex;
		data_buf[18] = c->acc_y_hex >> 8;
		data_buf[19] = c->acc_y_hex;
		data_buf[20] = c->acc_z_hex >> 8;
		data_buf[21] = c->acc_z_hex;
		data_buf[22] = d->acc_x_hex >> 8;
		data_buf[23] = d->acc_x_hex;
		data_buf[24] = d->acc_y_hex >> 8;
		data_buf[25] = d->acc_y_hex;
		data_buf[26] = d->acc_z_hex >> 8;
		data_buf[27] = d->acc_z_hex;
		data_buf[28] = rotation_x >> 8;
		data_buf[29] = rotation_x;
		data_buf[30] = rotation_y >> 8;
		data_buf[31] = rotation_y;
		data_buf[32] = rotation_z >> 8;
		data_buf[33] = rotation_z;
		SPI_Flash_Write((u8*)data_buf,current_address,34);
		current_address += 34;
		impact_flag = 1;
	}
}

void read_rotation(signed short *rotation_x, signed short *rotation_y, signed short *rotation_z) //read gyro data
{
	*rotation_x = mpu9250_read_x_high();
	*rotation_x = (*rotation_x<<8) + mpu9250_read_x_low();
	*rotation_y = mpu9250_read_y_high();
	*rotation_y = (*rotation_y<<8) + mpu9250_read_y_low();
	*rotation_z = mpu9250_read_z_high();
	*rotation_z = (*rotation_z<<8) + mpu9250_read_z_low();
}
