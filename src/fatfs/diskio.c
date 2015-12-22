/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <string.h>
#include "sd.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define ATA		0
#define MMC		1
#define USB		2

int diskio_sd_init_error;

/*-----------------------------------------------------------------------*/
/* Inicializes a Drive                                                    */

DSTATUS disk_initialize (BYTE drv)    /* Physical drive nmuber (0..) */
{
	diskio_sd_init_error = SD_Card_Init();
	if (diskio_sd_init_error == 0) return RES_OK;
	return STA_NOINIT;
	
//  if (SD_Init() == 0) return RES_OK;
 // return STA_NOINIT;
  
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	//if (1) {return 0;}
	return 0;//STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	if (count > 1) {
		SD_ErrorCode = SD_Read_Multi_Blocks(sector, buff, count);
	} else {
		SD_ErrorCode = SD_Read_Single_Block(sector, buff);
	}
	if (SD_ErrorCode == SD_OK) return RES_OK;
	return RES_ERROR;
	//return RES_PARERR;
}
