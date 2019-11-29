#include "spi1.h"
#include "delay.h"
#include "spi.h"

SPI_InitTypeDef  SPI_InitStructure;

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure); 
 
	SPI_Cmd(SPI1, ENABLE); 
	
	SPI1_ReadWriteByte(0xff);	 
}   
  
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
} 

u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) 
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData);
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); 				    
}

u16 SPI_FLASH_TYPE=W25Q64;

void SPI_Flash_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
	SPI1_Init();		  
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);
	SPI_FLASH_TYPE=SPI_Flash_ReadID();
}  

u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS=0;                        
	SPI1_ReadWriteByte(W25X_ReadStatusReg);      
	byte=SPI1_ReadWriteByte(0Xff);             
	SPI_FLASH_CS=1;                           
	return byte;   
} 

void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                    
	SPI1_ReadWriteByte(W25X_WriteStatusReg);   
	SPI1_ReadWriteByte(sr);          
	SPI_FLASH_CS=1;                          	      
}   

void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                          
    SPI1_ReadWriteByte(W25X_WriteEnable);  
	SPI_FLASH_CS=1;                              	      
} 
 
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                             
    SPI1_ReadWriteByte(W25X_WriteDisable);       
	SPI_FLASH_CS=1;                            	      
} 			    

u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;				    
	SPI1_ReadWriteByte(0x90);  
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;				    
	return Temp;
}   		    

void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            
    SPI1_ReadWriteByte(W25X_ReadData);           
    SPI1_ReadWriteByte((u8)((ReadAddr)>>16));     
    SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI1_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI1_ReadWriteByte(0XFF);   
    }
	SPI_FLASH_CS=1;                            	      
}  
 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                
	SPI_FLASH_CS=0;                           
    SPI1_ReadWriteByte(W25X_PageProgram);     
    SPI1_ReadWriteByte((u8)((WriteAddr)>>16));    
    SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);
	SPI_FLASH_CS=1;                      
	SPI_Flash_Wait_Busy();		
}

void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256;  	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;
	 	else
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;	
			if(NumByteToWrite>256)pageremain=256; 
			else pageremain=NumByteToWrite; 	
		}
	};	    
} 
 		   
u8 SPI_FLASH_BUF[4096];

void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) //write u8 array into W25Q starting from address WriteAddr
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	     

	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;  

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;
	while(1) 
	{	
    SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);			   
		if(NumByteToWrite==secremain)break;
		else
		{
			secpos++;
			secoff=0; 

		   	pBuffer+=secremain; 
			WriteAddr+=secremain;   
		   	NumByteToWrite-=secremain;				
			if(NumByteToWrite>4096)secremain=4096;	
			else secremain=NumByteToWrite;		
		}	 
	};	 	 
}

void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();             
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            
    SPI1_ReadWriteByte(W25X_ChipErase);       
	SPI_FLASH_CS=1;                              	      
	SPI_Flash_Wait_Busy();   				
}   

void SPI_Flash_Erase_Sector(u32 Dst_Addr)  //erase input sector number on W25Q 
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();             
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                        
    SPI1_ReadWriteByte(W25X_SectorErase);    
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));     
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI1_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                           	      
    SPI_Flash_Wait_Busy();   				 
}  

void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);  
}  

void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                       
    SPI1_ReadWriteByte(W25X_PowerDown);     
	SPI_FLASH_CS=1;                          	      
    delay_us(3);                              
}   

void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS=0;                           
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);   
	  SPI_FLASH_CS=1;                              
    delay_us(3);                             
}   
