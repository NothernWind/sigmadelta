/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef FSMC_H
#define FSMC_H

#include "main.h"

#define FSMC_REG      (*((volatile unsigned short *) 0x60000000))
#define FSMC_RAM      (*((volatile unsigned short *) 0x60020000))

#define mFSMC_WriteIndex(value)	FSMC_REG = value;
#define mFSMC_WriteData(value)	FSMC_RAM = value;
#define mFSMC_ReadData()			LCD_RAM

#define mFSMC_WriteReg(reg, ram) {\
  mFSMC_WriteIndex(reg);\
  mFSMC_WriteData(ram);\
}

#define mFSMC_ReadReg(reg, ram) {\
	mFSMC_WriteIndex(reg);\
	ram = mFSMC_ReadData();\
}

extern volatile char DMA_TransferComplete;

void FSMC_init(void);
void FSMC_DMA_Write(const unsigned short *buf, int size);

#endif // FSMC_H
