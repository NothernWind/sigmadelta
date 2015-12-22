/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "i2s.h"

char I2S_Transfer_Complete;

void DMA1_Stream7_IRQHandler(void)
{
	if (DMA1->HISR & DMA_HISR_TCIF7) {
		DMA1->HIFCR = 
			DMA_HIFCR_CTCIF7 | 
			DMA_HIFCR_CHTIF7 | 
			DMA_HIFCR_CTEIF7 |
			DMA_HIFCR_CDMEIF7 |
			DMA_HIFCR_CFEIF7;
		//MARK_OFF
		I2S_Transfer_Complete = 1;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 I2SPLLN = 429; I2SPLLP = 2; F(I2S) = 214.5 MHz
 +--------+--------+-----+----------+----------+
 | Fs, Hz | I2SDIV | ODD |  Fs, Hz  | Error, % |
 +--------+--------+-----+----------+----------+
 |  11025 |   38   |  0  | 11024,88 |  0,0011  |
 |  22050 |   19   |  0  | 22049,75 |  0,0011  |
 |  44100 |    9   |  1  | 44099,51 |  0,0011  |
 +--------+--------+-----+----------+----------+

 I2SPLLN = 344; I2SPLLP = 2; F(I2S) = 172 MHz
 +--------+--------+-----+----------+----------+
 | Fs, Hz | I2SDIV | ODD |  Fs, Hz  | Error, % |
 +--------+--------+-----+----------+----------+
 |   8000 |   42   |  0  |  7998,51 |  0,0186  |
 |  16000 |   21   |  0  | 15997,02 |  0,0186  |
 |  24000 |   14   |  0  | 23995,54 |  0,0186  |
 |  32000 |   10   |  1  | 31994,05 |  0,0186  |
 |  48000 |    7   |  0  | 47991,07 |  0,0186  |
 +--------+--------+-----+----------+----------+

 --------------------------------------------------------------------
 */
int I2S_Init(int smprate)
{
	int err = 0;
	I2S_Transfer_Complete = 0;
	
	switch(smprate) {
		case 8000:
		case 16000:
		case 24000:
		case 32000:
		case 48000:
			RCC->PLLI2SCFGR = (2UL << 28UL) | (344UL << 6);
			break;
		case 11025:
		case 22050:
		case 44100:
			RCC->PLLI2SCFGR = (2UL << 28UL) | (429UL << 6);
			break;
		default:
			err = -1;
			break;
	}
	
	if (err != 0) return err;
	
	RCC->CR |= RCC_CR_PLLI2SON;
	while (!(RCC->CR & RCC_CR_PLLI2SRDY));
	
	// Затактируем SPI3
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;	
	
	// Настроим нужное значение делителя
	switch(smprate) {
		case 8000:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 42;
			break;
		case 16000:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 21;
			break;
		case 24000:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 14;
			break;
		case 32000:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 10;
			SPI3->I2SPR |= SPI_I2SPR_ODD;
			break;
		case 48000:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 7;
			break;
		case 11025:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 38;
			break;
		case 22050:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 19;
			break;
		case 44100:
			SPI3->I2SPR = 0;
			SPI3->I2SPR = 9;
			SPI3->I2SPR |= SPI_I2SPR_ODD;
			break;
		default:
			err = -1;
			break;
	}
	
	if (err != 0) return err;

	// Выход MCK Всегда будер разрешён
	SPI3->I2SPR |= SPI_I2SPR_MCKOE;
	
	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SSTD_1;
	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1;	

	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SE;

	SPI3->CR2 |= SPI_CR2_TXDMAEN;
	
	DMA1_Stream7->PAR = SPI3_BASE | 0x0C;
	DMA1_Stream7->CR |= 
		DMA_SxCR_MSIZE_0 |
		DMA_SxCR_PSIZE_0 |
		DMA_SxCR_MINC |
		DMA_SxCR_DIR_0 |
		DMA_SxCR_TCIE;
		
	NVIC->ISER[1] = 0x8000;	
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void I2S_DeInit(void)
{
	// Сначала запрещаем прерывание от DMA1 Stream7
	NVIC->ICER[1] = 0x8000;
	
	// Выключаем DMA1 Stream7
	DMA1_Stream7->CR = 0;
	DMA1_Stream7->PAR = 0;
	
	// Выключаем I2S3
	SPI3->CR2 = 0;
	SPI3->I2SCFGR  = 0;
	SPI3->I2SPR = 0;
	
	// Запрещаем тактирование I2S3
	RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
	
	// Выключаем I2SPLL
	RCC->CR &= ~RCC_CR_PLLI2SON;
	
	// Обнуляем Значения делителей и множителей
	RCC->PLLI2SCFGR = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void I2S_Send_Audio_Buffer(const unsigned short * buf, int size)
{
	DMA1_Stream7->M0AR = (unsigned int)(buf);
	
	DMA1_Stream7->NDTR = size;
	
	DMA1->HIFCR = 
		DMA_HIFCR_CTCIF7 | 
		DMA_HIFCR_CHTIF7 | 
		DMA_HIFCR_CTEIF7 |
		DMA_HIFCR_CDMEIF7 |
		DMA_HIFCR_CFEIF7; 
		
	DMA1_Stream7->CR |= DMA_SxCR_EN;
	
}
