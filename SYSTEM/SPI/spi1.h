#ifndef __SPI1_H
#define __SPI1_H
#include "sys.h"
#include "spi.h"

#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
 

extern u16 SPI_FLASH_TYPE;	

#define	SPI_FLASH_CS PAout(2)  			 

extern u8 SPI_FLASH_BUF[4096];

#define FLASH_ID 0XEF14

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F  
 				  	    													  
void SPI1_Init(void);		
void SPI1_SetSpeed(u8 SpeedSet); 
u8 SPI1_ReadWriteByte(u8 TxData);
void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);  	  
u8	 SPI_Flash_ReadSR(void);    
void SPI_FLASH_Write_SR(u8 sr);  
void SPI_FLASH_Write_Enable(void); 
void SPI_FLASH_Write_Disable(void);
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);  
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void SPI_Flash_Erase_Chip(void);    	 
void SPI_Flash_Erase_Sector(u32 Dst_Addr);
void SPI_Flash_Wait_Busy(void);       
void SPI_Flash_PowerDown(void);         
void SPI_Flash_WAKEUP(void);			 
		 
#endif
