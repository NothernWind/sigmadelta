/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "fsmc.h"

#define mDMAClearFlags()\
DMA2->HIFCR = DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 | \
DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTCIF4;

volatile char DMA_TransferComplete;

void DMA2_Stream4_IRQHandler(void)
{
	if (DMA2->HISR & DMA_HISR_TCIF4) {
		mDMAClearFlags();
		DMA_TransferComplete = 1;
	}
}

/*!
 --------------------------------------------------------------------
 \brief Инициализация FSMC для работа с LCD
 
 Config:
 Bank - 1
 Data Address Mux - Disable
 Memory Type - SRAM
 Memory Data Width - 16b
 Burst Access Mode - Disable
 Wait Signal Polarity - Low
 Warp Mode - Disable
 Wait Signal Active - Before Wait State
 Write Operation - Enable
 Wait Signal - Disable
 Asynchronous Wait - Disable
 Extended Mode - Disable
 Write Burst - Enable
 
 Timing Config:
 Address Stup Time = 0
 Adress Hold Time = 0
 Data Setup Time = 8
 Bus Turn Around Duration = 1
 CLK Div = 2
 Data Latency = 1
 Access Mode = A
 
 --------------------------------------------------------------------
 */
void FSMC_init(void) 
{
	DMA_TransferComplete = 0;
	
	FSMC_Bank1->BTCR[0] = 0x00001011;
	FSMC_Bank1->BTCR[1] = 0x01111080;
	
	// Предварительно настроим DMA.
	// Адрес куда будут сливаться данные 0x60020000
	// Направление: mem-to-mem
	// Размер данных: WORD(16-bit)
	// Periph increment: enable;
	DMA2_Stream4->M0AR = 0x60020000;
	DMA2_Stream4->CR |= 
		DMA_SxCR_MSIZE_0 |
		DMA_SxCR_PSIZE_0 |
		DMA_SxCR_PINC |
		DMA_SxCR_DIR_1 | 
		DMA_SxCR_TCIE;
	// Прерывание. DMA2 Stream 4 NVIC POS 60
	NVIC->ISER[1] = 0x10000000;
}

/*!
 --------------------------------------------------------------------
 \brief Настройка DMA.
 
 Параметры следующие:
 Channel 0;
 MSIZE = 16 bit;
 PSIZE = 16 bit;
 DIR = MemToMem
 DMA single buffer mode srcdst dep 
 +----------+----------------------+------------+-------------+
 | DIR[1:0] |       Direction      |   Source   | Destination |
 +----------+----------------------+------------+-------------+
 |    00    | Peripheral-to-memory |  DMA_SxPAR |  DMA_SxM0AR |
 +----------+----------------------+------------+-------------+
 |    01    | Memory-to-peripheral | DMA_SxM0AR |  DMA_SxPAR  |
 +----------+----------------------+------------+-------------+
 |    10    |   Memory-to-memory   |  DMA_SxPAR |  DMA_SxM0AR |
 +----------+----------------------+------------+-------------+
 p. 308
 
 Memory-to-memory mode
 The DMA channels can also work without being triggered by a request 
 from a peripheral. This is the memory-to-memory mode, described in 
 Figure 38. (Reference manual p.308)
 
 When the stream is enabled by setting the Enable bit (EN) in the 
 DMA_SxCR register, the stream immediately starts to fill the FIFO 
 up to the threshold level. When the threshold level is reached, 
 the FIFO contents are drained and stored into the destination.
 
 The transfer stops once the DMA_SxNDTR register reaches zero or 
 when the EN bit in the DMA_SxCR register is cleared by software.
 
 The stream has access to the AHB source or destination port only 
 if the arbitration of the corresponding stream is won. This arbitration 
 is performed using the priority defined for each stream using the 
 PL[1:0] bits in the DMA_SxCR register.
 
 Note: When memory-to-memory mode is used, the Circular and direct 
       modes are not allowed. Only the DMA2 controller is able to 
       perform memory-to-memory transfers.
 
 --------------------------------------------------------------------
 */

void FSMC_DMA_Write(const unsigned short *buf, int size)
{
	DMA2_Stream4->PAR = (unsigned int)(buf);
	DMA2_Stream4->NDTR = size;
	
	mDMAClearFlags();
	
	DMA2_Stream4->CR |= DMA_SxCR_EN;
}
