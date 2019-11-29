#ifndef __SPI_H
#define __SPI_H			    
#include "sys.h" 
#include "spi1.h"		   

#define	H3LIS_CS_top 		PBout(8)  		
#define	H3LIS_CS_left 		PBout(9)	
#define	H3LIS_CS_right 		PBout(10)
#define	H3LIS_CS_rear 		PBout(11)
#define	MPU9250_CS 		PBout(7)

typedef struct acc
{
  u8 acc_id;  
	signed short acc_x_hex, acc_y_hex, acc_z_hex;
} acc;

extern u8 impact_num;

void SPI_bus_Init(void);
void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
u8 H3LIS_Read_WhoIam(u8 id);
void H3LIS_setpower_outputfrequency_axis(u8 id);
void H3LIS_setscale(u8 id);
u8 read_z_low(u8 id);
u8 read_z_high(u8 id);
u8 read_x_low(u8 id);
u8 read_x_high(u8 id);
u8 read_y_low(u8 id);
u8 read_y_high(u8 id);
u8 mpu9250_read_whoim(void);
void mpu9250_config(void);
void mpu9250_gyro_config(void);
void H3LIS_set_id(acc *a, u8 id);
void read_acc(acc *a);
void check_impact(acc *a, acc *b, acc *c, acc *d, u16 time_ms, u8 time_sec, u8 time_min, signed short rotation_x, signed short rotation_y, signed short rotation_z);
void SPI1_Init(void);
u8 SPI1_ReadWriteByte(u8 TxData);
void SPI1_SetSpeed(u8 SpeedSet);
u16 SPI_Flash_ReadID(void);
u8 mpu9250_read_x_high(void);
u8 mpu9250_read_x_low(void);
u8 mpu9250_read_y_high(void);
u8 mpu9250_read_y_low(void);
u8 mpu9250_read_z_high(void);
u8 mpu9250_read_z_low(void);
void read_rotation(signed short *rotation_x, signed short *rotation_y, signed short *rotation_z);

#endif
