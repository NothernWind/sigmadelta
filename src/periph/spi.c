/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "spi.h"

t_spi_flag SPI1_Soft_Slag;
t_spi_flag SPI2_Soft_Slag;

unsigned char null_data;
unsigned char null_rx_data;
unsigned char null_tx_data;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void DMA1_Stream3_IRQHandler(void)
{
	if (DMA1->LISR & DMA_LISR_TCIF3) {
		DMA1->LIFCR = 
			DMA_LIFCR_CTCIF3 | 
			DMA_LIFCR_CHTIF3 | 
			DMA_LIFCR_CTEIF3 |
			DMA_LIFCR_CDMEIF3 |
			DMA_LIFCR_CFEIF3;

		SPI2_Soft_Slag.bits.RXRDY = 1;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void DMA1_Stream4_IRQHandler(void)
{
	if (DMA1->HISR & DMA_HISR_TCIF4) {
		DMA1->HIFCR = 
			DMA_HIFCR_CTCIF4 | 
			DMA_HIFCR_CHTIF4 | 
			DMA_HIFCR_CTEIF4 |
			DMA_HIFCR_CDMEIF4 |
			DMA_HIFCR_CFEIF4;
		
		SPI2_Soft_Slag.bits.TXRDY = 1;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
/*!
 ********************************************************************
 * \brief	Настройка SPI2 + DMA1
 *
 * SPI2 DMA1 Channel0
 * Rx   Stream3		nvic 14
 * Tx   Stream4		nvic 15
 * 
 ********************************************************************
 */
void SPI2_DMA_Init(void)
{
	// Инициализация SPI
	null_data = 0xFF;
	null_rx_data = 0xFF;
	null_tx_data = 0xFF;
	SPI2_Soft_Slag.all = 0;
// CHPA = 0, CPOL = 0, BR = 0, DFF = 0;
	SPI2->CR1 |= SPI_CR1_SSM;
	SPI2->CR1 |= SPI_CR1_SSI;
	SPI2->CR1 |= SPI_CR1_MSTR;
	SPI2->CR1 |= SPI_CR1_SPE;
	
	SPI2->CR2 |= 
		SPI_CR2_TXDMAEN |
		SPI_CR2_RXDMAEN;

	/*
	 * Тут нужно предварительно инициализировать DMA, так что бы он 
	 * знал в каком напралении с каким каналом и каким регистрм он
	 * работает, а так же разрешить ему прерывание.
	 */

	// Инициализация Rx DMA

	DMA1_Stream3->PAR = SPI2_BASE | 0x0C;	
	
	DMA1_Stream3->CR |= 
		DMA_SxCR_TCIE;
		
	// Инициализация TX DMA
	
	DMA1_Stream4->PAR = SPI2_BASE | 0x0C;
	
	DMA1_Stream4->CR |= 
		DMA_SxCR_DIR_0 | 
		DMA_SxCR_TCIE;

	// Разрешаем прерывание
	NVIC->ISER[0] = 0x0000C000;
}

/*!
 --------------------------------------------------------------------
 \brief Эта фукнция переключает SPI на низкую скорость.
 
 В этой функции делитель BR[5:2] из регистра CR1 настраивается на 
 Fpclk/128, в итоге частота тактирования получается примерно 330 kHz
 
 --------------------------------------------------------------------
 */
void SPI2_Set_Low_Speed(void) 
{
	SPI2->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_2;
}

/*!
 --------------------------------------------------------------------
 \brief Эта функция переключает SPI на высокую скорость (примерно 5 MHz) 

 --------------------------------------------------------------------
 */
void SPI2_Set_High_Speed(void) 
{
	//SPI2->CR1 &= ~(SPI_CR1_BR_2);
	SPI2->CR1 &= ~(SPI_CR1_BR_1 | SPI_CR1_BR_2);
}

/*!
 --------------------------------------------------------------------
 \brief Эта функция переключает SPI на среднюю скорость (примерно 5 MHz) 

 --------------------------------------------------------------------
 */
void SPI2_Set_Medium_Speed(void) 
{
	SPI2->CR1 &= ~(SPI_CR1_BR_2);
	//SPI2->CR1 &= ~(SPI_CR1_BR_1 | SPI_CR1_BR_2);
}

/*!
 --------------------------------------------------------------------
 \brief Запускает транзакцию в обе стороны.
 
 \param	tx_buf - указатель на буфер данных которые нужно отправить
 \param	rx_buf - указатель на буфер в который нужно сложить принятые 
 			данные
 \param	size - длинна транзакции.
 
 \note	Об окончании транзакции говорит флаг SPI2_Soft_Slag.bits.RXRDY
 --------------------------------------------------------------------
 */
void SPI2_DMA_Transfer(const unsigned char * tx_buf, 
	unsigned char * rx_buf, int size)
{
	DMA1_Stream3->CR |= DMA_SxCR_MINC;
	DMA1_Stream4->CR |= DMA_SxCR_MINC;
	
	DMA1_Stream3->M0AR = (unsigned int)rx_buf;
	DMA1_Stream3->NDTR = size;
	
	DMA1_Stream4->M0AR = (unsigned int)tx_buf;
	DMA1_Stream4->NDTR = size;
	
	DMA1->LIFCR = 
		DMA_LIFCR_CTCIF3 | 
		DMA_LIFCR_CHTIF3 | 
		DMA_LIFCR_CTEIF3 |
		DMA_LIFCR_CDMEIF3 |
		DMA_LIFCR_CFEIF3; 
	
	DMA1->HIFCR = 
		DMA_HIFCR_CTCIF4 | 
		DMA_HIFCR_CHTIF4 | 
		DMA_HIFCR_CTEIF4 |
		DMA_HIFCR_CDMEIF4 |
		DMA_HIFCR_CFEIF4; 
		
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

/*!
 --------------------------------------------------------------------
 \brief Запускает считывание данных из SPI
 
 \param	rx_buf - указатель на буфер в который нужно сложить принятые 
        данные
 \param	size - длинна транзакции.
 
 \note	Об окончании транзакции говорит флаг SPI2_Soft_Slag.bits.RXRDY
 --------------------------------------------------------------------
 */
void SPI2_DMA_Rx_Data(unsigned char * rx_buf, int size)
{
	DMA1_Stream3->CR |= DMA_SxCR_MINC;
	DMA1_Stream4->CR &= ~DMA_SxCR_MINC;
	
	DMA1_Stream3->M0AR = (unsigned int)rx_buf;
	DMA1_Stream3->NDTR = size;
	
	DMA1_Stream4->M0AR = (unsigned int)(&null_data);
	DMA1_Stream4->NDTR = size;
	
	DMA1->LIFCR = 
		DMA_LIFCR_CTCIF3 | 
		DMA_LIFCR_CHTIF3 | 
		DMA_LIFCR_CTEIF3 |
		DMA_LIFCR_CDMEIF3 |
		DMA_LIFCR_CFEIF3; 
	
	DMA1->HIFCR = 
		DMA_HIFCR_CTCIF4 | 
		DMA_HIFCR_CHTIF4 | 
		DMA_HIFCR_CTEIF4 |
		DMA_HIFCR_CDMEIF4 |
		DMA_HIFCR_CFEIF4; 
		
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

/*!
 --------------------------------------------------------------------
 \brief Запускает передачу данных в SPI

 \param	tx_buf - указатель на буфер данных которые нужно отправить
 \param	size - длинна транзакции.

 \note	Об окончании транзакции говорит флаг SPI2_Soft_Slag.bits.RXRDY
 --------------------------------------------------------------------
 */
void SPI2_DMA_Tx_Data(const unsigned char * tx_buf, int size)
{
	DMA1_Stream3->CR &= ~DMA_SxCR_MINC;
	DMA1_Stream4->CR |= DMA_SxCR_MINC;
	
	DMA1_Stream3->M0AR = (unsigned int)(&null_data);
	DMA1_Stream3->NDTR = size;
	
	DMA1_Stream4->M0AR = (unsigned int)tx_buf;
	DMA1_Stream4->NDTR = size;
	
	DMA1->LIFCR = 
		DMA_LIFCR_CTCIF3 | 
		DMA_LIFCR_CHTIF3 | 
		DMA_LIFCR_CTEIF3 |
		DMA_LIFCR_CDMEIF3 |
		DMA_LIFCR_CFEIF3; 
	
	DMA1->HIFCR = 
		DMA_HIFCR_CTCIF4 | 
		DMA_HIFCR_CHTIF4 | 
		DMA_HIFCR_CTEIF4 |
		DMA_HIFCR_CDMEIF4 |
		DMA_HIFCR_CFEIF4; 
		
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

/*!
 --------------------------------------------------------------------
 \brief Просто выдаёт clocks тактов на линии SPI_SCK
 
 --------------------------------------------------------------------
 */
void SPI2_DMA_Clocking(int clocks)
{
	DMA1_Stream3->CR &= ~DMA_SxCR_MINC;
	DMA1_Stream4->CR &= ~DMA_SxCR_MINC;
	
	DMA1_Stream3->M0AR = (unsigned int)(&null_rx_data);
	DMA1_Stream3->NDTR = clocks;
	
	DMA1_Stream4->M0AR = (unsigned int)(&null_tx_data);
	DMA1_Stream4->NDTR = clocks;
	
	DMA1->LIFCR = 
		DMA_LIFCR_CTCIF3 | 
		DMA_LIFCR_CHTIF3 | 
		DMA_LIFCR_CTEIF3 |
		DMA_LIFCR_CDMEIF3 |
		DMA_LIFCR_CFEIF3; 
	
	DMA1->HIFCR = 
		DMA_HIFCR_CTCIF4 | 
		DMA_HIFCR_CHTIF4 | 
		DMA_HIFCR_CTEIF4 |
		DMA_HIFCR_CDMEIF4 |
		DMA_HIFCR_CFEIF4; 
		
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;	
}
