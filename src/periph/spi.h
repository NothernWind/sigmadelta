/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef SPI_H
#define SPI_H

#include "main.h"

typedef union _t_spi_flag {
	unsigned char all;
	struct {
		unsigned BUSY:1;
		unsigned TRRDY:1;
		unsigned TXRDY:1;
		unsigned RXRDY:1;
		unsigned :4;
	} bits;
} t_spi_flag;

extern t_spi_flag SPI1_Soft_Slag;
extern t_spi_flag SPI2_Soft_Slag;

void SPI2_DMA_Init(void);
void SPI2_Set_Low_Speed(void);
void SPI2_Set_High_Speed(void);
void SPI2_DMA_Transfer(const unsigned char *tx_buf, unsigned char *rx_buf, int size);
void SPI2_DMA_Rx_Data(unsigned char * rx_buf, int size);
void SPI2_DMA_Tx_Data(const unsigned char * tx_buf, int size);
void SPI2_DMA_Clocking(int clocks);
void SPI2_Set_Medium_Speed(void);

#endif // SPI_H
