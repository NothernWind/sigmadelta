/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef I2S_H
#define I2S_H

#include "main.h"

extern char I2S_Transfer_Complete;

int I2S_Init(int smprate);
void I2S_DeInit(void);
void I2S_Send_Audio_Buffer(const unsigned short * buf, int size);

#endif
