/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "rcc.h"

/*!
 --------------------------------------------------------------------
 \brief Вкулючаем тактирование для всей нужной нам периферии.
 
 --------------------------------------------------------------------
 */
void RCC_Enable_Clocks(void)
{
	RCC->AHB1ENR |= 
		RCC_AHB1ENR_GPIOAEN |
		RCC_AHB1ENR_GPIOBEN |
		RCC_AHB1ENR_GPIOCEN |
		RCC_AHB1ENR_GPIODEN |
		RCC_AHB1ENR_GPIOEEN | 
		RCC_AHB1ENR_DMA1EN |
		RCC_AHB1ENR_DMA2EN;
	
	RCC->AHB3ENR |= 
		RCC_AHB3ENR_FSMCEN;
	
	RCC->APB1ENR |= 
		RCC_APB1ENR_SPI2EN | 
		RCC_APB1ENR_I2C1EN |
		RCC_APB1ENR_SPI3EN |
		RCC_APB1ENR_TIM7EN;

	RCC->APB2ENR |= 
		RCC_APB2ENR_SPI1EN;
}
