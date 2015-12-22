/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "i2c.h"

// Variables --------------------------------------------------------
unsigned char I2C1_Device_Addr;
volatile char I2C1_Trasfer_Complete;
volatile int I2C_Status = 0;

int i2c_tr_step = 0;
char direction = 0;

unsigned char regaddr;
unsigned char txdata;
unsigned char rxdata;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void I2C1_EV_IRQHandler(void) 
{
	volatile unsigned short sr1;
	volatile unsigned short sr2;
	
	sr1 = I2C1->SR1;
	sr2 = I2C1->SR2;
	
	switch (i2c_tr_step) {
		
		// Отправа заголовка ----------------------------------------
		case 0: // START передан
			// Отправляем адрес устройства
			if (sr1 & I2C_SR1_SB) {
				I2C1->DR = I2C1_Device_Addr;
				i2c_tr_step++;
				break;
			}
			I2C_Status = -1;
			break;
		
		case 1: // Адрес устройства отправлен 
			// Отправляем адрес регистра.
			if (sr1 & I2C_SR1_ADDR) {
				I2C1->DR = regaddr;
				i2c_tr_step++;
				break;
			}
			I2C_Status = -2;
			break;
		
		// Чтение или Запись ----------------------------------------
		case 2: // Адрес регистра отправлен
			// И теперь ветвление
			if (direction == 0) { // Если передаём данные устройству
				// Просто отправляем данные
				if (sr1 & I2C_SR1_TXE) {
					I2C1->DR = txdata;
					i2c_tr_step++;
					break;
				}
			} else { // Если принимаем данные из устройства
				if (sr1 & I2C_SR1_BTF) {
					I2C1->CR1 |= I2C_CR1_START;
					i2c_tr_step = 4;
					break;
				}
			}
			I2C_Status = -3;
			break;
		
		// Запись и отправка стоп -----------------------------------
		case 3: // Отправка данных закончена
			// отправляем STOP
			if (sr1 & I2C_SR1_BTF) {
				I2C1->CR1 |= I2C_CR1_STOP;
				i2c_tr_step = 7;
				break;
			}
			I2C_Status = -4;
			break;
		
		// Чтение ---------------------------------------------------
		case 4: // Повторный старт отправлен
			// Отправляем адрес устройства + бит чтения
			if (sr1 & I2C_SR1_SB) {
				I2C1->DR = I2C1_Device_Addr | 0x01;
				i2c_tr_step++;
				break;
			}
			I2C_Status = -5;
			break;
		
		case 5: // Адрес устройсва отправлен
			// Принимаем данные
			if (sr1 & I2C_SR1_ADDR) {
				i2c_tr_step++;
				break;
			}
			I2C_Status = -6;
			break;
			
		case 6: // Данные приняты вычитаем их и отправим стоп
			if (sr1 & I2C_SR1_RXNE) {
				rxdata = I2C1->DR;
				I2C1->CR1 |= I2C_CR1_STOP;
				i2c_tr_step++;
				break;
			}
			I2C_Status = -7;
			break;
		
		// Стоп -----------------------------------------------------
		case 7: // STOP отправлен.
			while (I2C1->CR1 & I2C_CR1_STOP);
		
			I2C_Status = 0;
			I2C1_Trasfer_Complete = 1;

			break;
			
		default: 
			I2C_Status = -9; 
			break;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void I2C_Init(void)
{
	// Настроим I2C
	I2C1->CR2 |= 
		I2C_CR2_FREQ_5 |
		I2C_CR2_FREQ_3 |
		I2C_CR2_FREQ_1;
	
	I2C1->CR2 |= 
		I2C_CR2_ITBUFEN | 
		I2C_CR2_ITEVTEN;
	
	I2C1->CCR = 0x8022;
	
	I2C1->TRISE = 41;
	
	// И Прерывание
	NVIC->ISER[0] = 0x80000000;

}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void I2C_IRQ_Write(unsigned char addr, unsigned char data)
{
	if (I2C_Status != 0) return;
	I2C_Status = 1;
	i2c_tr_step = 0;
	regaddr = addr;
	txdata = data;
	direction = 0;
	I2C1->CR1 |= I2C_CR1_PE;
	I2C1->CR1 |= I2C_CR1_START;
}
