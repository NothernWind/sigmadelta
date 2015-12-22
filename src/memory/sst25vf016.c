/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "sst25vf016.h"

// Variables --------------------------------------------------------
unsigned char header[8];
t_sst25_status sst25_status;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void sst25_read_block(unsigned int blk_addr, 
	unsigned char * b_addr, int size)
{
	LED4_ON
	header[0] = 0x03;
	header[1] = (unsigned char)((blk_addr & 0x00FF0000) >> 16);
	header[2] = (unsigned char)((blk_addr & 0x0000FF00) >> 8);
	header[3] = (unsigned char)((blk_addr & 0x000000FF));
	
	EEPROM_CS_ON
	
	SPI2_DMA_Tx_Data((unsigned char *)(&header[0]), 4);
	
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	
	SPI2_DMA_Rx_Data(b_addr, size);
	
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	
	EEPROM_CS_OFF
	LED4_OFF
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
unsigned char sst25_read_status(void)
{
	static unsigned char tx_data[2];
	static unsigned char rx_data[2];
	EEPROM_CS_ON
	tx_data[0] = 0x05;
	tx_data[1] = 0xFF;
	SPI2_DMA_Transfer(tx_data, rx_data, 2);
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	EEPROM_CS_OFF
	return rx_data[1];
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
/*!
 ********************************************************************
 * \brief
 *
 * \param	command: 
 *          WREN       06h
 *          WRDI       04h
 *          Chip-Erase 60h
 *          EWSR       50h
 ********************************************************************
 */
void sst25_write_command(unsigned char command)
{
	static unsigned char cmd;
	cmd = command;
	EEPROM_CS_ON
	SPI2_DMA_Tx_Data((unsigned char *)(&cmd), 1);
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	EEPROM_CS_OFF	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void sst25_write_status(unsigned char stat) 
{
	static unsigned char cmd[2];
	cmd[0] = SST25_WRSR;
	cmd[1] = stat;
	EEPROM_CS_ON
	SPI2_DMA_Tx_Data((unsigned char *)(&cmd[0]), 2);
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	EEPROM_CS_OFF	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void sst25_byte_program(unsigned int addr, unsigned char b)
{
	header[0] = 0x02;
	header[1] = (unsigned char)((addr & 0x00FF0000) >> 16);
	header[2] = (unsigned char)((addr & 0x0000FF00) >> 8);
	header[3] = (unsigned char)((addr & 0x000000FF));	
	header[4] = b;
	
	EEPROM_CS_ON
	
	SPI2_DMA_Tx_Data((unsigned char *)(&header[0]), 5);
	
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
	
	EEPROM_CS_OFF
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void write_enable(void)
{
	sst25_write_command(SST25_WREN);
	
	sst25_status.all = sst25_read_status();
	while (sst25_status.bits.WEL == 0) {
		sst25_status.all = sst25_read_status();
	}	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void wait_busy(void)
{
	sst25_status.all = sst25_read_status();
	while(sst25_status.bits.BUSY == 1) {
		sst25_status.all = sst25_read_status();
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void sst25_full_erase(void)
{
	LED4_ON
	sst25_write_command(SST25_WREN);
	
	write_enable();
	
	sst25_write_status(0);
	
	sst25_status.all = sst25_read_status();
	while (sst25_status.all != 0) {
		sst25_status.all = sst25_read_status();
	}	
	
	write_enable();
	
	sst25_write_command(SST25_CHIP_ERASE);
	
	wait_busy();
	LED4_OFF
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void sst25_write_block(unsigned int addr, const unsigned char * block, int size)
{
	int i;
	
	LED4_ON
	
	sst25_write_command(SST25_WREN);
	
	write_enable();
	
	sst25_write_status(0);
	
	sst25_status.all = sst25_read_status();
	while (sst25_status.all != 0) {
		sst25_status.all = sst25_read_status();
	}
	
	for (i = 0; i < size; i++) {
		write_enable();
		sst25_byte_program(addr + i, block[i]);
		wait_busy();
	}
	LED4_OFF	
}
