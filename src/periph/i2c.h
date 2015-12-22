/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef I2C_H
#define I2C_H

#include "main.h"

// I2C Errors
#define I2C_START_ERROR		-1	// Ошибка отправки стартового бита
#define I2C_DEV_ADDR_ERROR	-2	// Ошибка отправки адреса устройства
#define I2C_POINTER_ERROR	-3	// Ошибка отправки адреса регистра
#define I2C_SEND_DATA_ERROR	-4	// Ошибка отправки данных
#define I2C_RSTART_ERROR	-5	// Ошибка отправки повторного старта
#define I2C_RADDR_ERROR		-6	// Ошибка отправки адреса на чтение
#define I2C_RCV_DATA_ERROR	-7	// Ошибка приёма данных
#define I2C_UNKNOWN_ERROR	-9	// Неизвесная ошибка

extern unsigned char I2C1_Device_Addr;
extern volatile char I2C1_Trasfer_Complete;
extern volatile int I2C_Status;

void I2C_Init(void);
void I2C_IRQ_Write(unsigned char addr, unsigned char data);


#endif // I2C_H
