/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef SST25VF016_H
#define SST25VF016_H

#include "main.h"

#define SST25_READ			0x03
#define SST25_HS_READ		0x0B
#define SST25_4K_ERASE		0x20
#define SST25_32K_ERASE		0x52
#define SST25_64K_ERASE		0xD8
#define SST25_CHIP_ERASE	0x60
#define SST25_BYTE_PROGRAM	0x02
#define SST25_AAI_PROGRAM	0xAD
#define SST25_RDSR			0x05
#define SST25_EWSR			0x50
#define SST25_WRSR			0x01
#define SST25_WREN			0x06
#define SST25_WRDI			0x04	
#define SST25_RDID			0x90	
#define SST25_JEDEC_ID		0x9F
#define SST25_EBSY			0x70
#define SST25_DBSY			0x80


/*!
 ********************************************************************
 * \brief	SOFTWARE STATUS REGISTER
 *
 * \par		BUSY: 1 = Internal Write operation is in progress;
 * 			      0 = No internal Write operation is in progress
 * \par		WEL:  1 = Device is memory Write enabled
 * 
 ********************************************************************
 */
typedef union _t_sst25_status {
	unsigned char all;
	struct {
		unsigned BUSY:1;
		unsigned WEL:1;
		unsigned BP:4;
		unsigned AAI:1;
		unsigned PBL:1;
	} bits;
	struct {
		unsigned :2;
		unsigned BP0:1;
		unsigned BP1:1;
		unsigned BP2:1;
		unsigned BP3:1;
		unsigned :2;
	} BPn;
} t_sst25_status;

extern t_sst25_status sst25_status;

void sst25_read_block(unsigned int blk_addr, unsigned char * b_addr, int size);
unsigned char sst25_read_status(void);
void sst25_write_command(unsigned char command);
void sst25_byte_program(unsigned int addr, unsigned char b);
void sst25_write_status(unsigned char stat);
void sst25_write_block(unsigned int addr, const unsigned char * block, int size);
void sst25_full_erase(void);

#endif // SST25VF016_H
