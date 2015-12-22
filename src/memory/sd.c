/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "sd.h"

// Variables --------------------------------------------------------
unsigned char SD_Command[6];
unsigned char SD_CSD_Register[17];
unsigned char SD_CID_Register[17];

unsigned short sd_crc_16;

unsigned char SD_R1_Response;
unsigned int SD_R7_Response;

SD_Info	SD_CardInfo;
int SD_ErrorCode;
unsigned int SD_OCR_Register;

// Code section -----------------------------------------------------

/*!
 --------------------------------------------------------------------
 \brief Отправка комманды в SD-карту
 
 \param	cmd - Код команды
 \param	arg - Аргумент
 
 --------------------------------------------------------------------
 */
void SD_Send_Command(unsigned char cmd, unsigned int arg)
{
	SD_Command[0] = cmd | 0x40;
	SD_Command[1] = (unsigned char)(arg >> 24);
	SD_Command[2] = (unsigned char)(arg >> 16);
	SD_Command[3] = (unsigned char)(arg >> 8);
	SD_Command[4] = (unsigned char)(arg);
	
	SD_Command[5] = ((cmd == CMD8) ? 0x87 : (cmd == CMD0) ? 0x95 : 0xFF);
	
	SPI2_DMA_Tx_Data(SD_Command, 6);
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;
}

/*!
 --------------------------------------------------------------------
 \brief Ожидание ответа от SD-карты
 
 --------------------------------------------------------------------
 */
unsigned char SD_Get_Status(int timeout)
{
	unsigned char R1_Response;
	while (timeout--) {
		SPI2_DMA_Rx_Data((unsigned char *)(&R1_Response), 1);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		if (R1_Response != 0xFF) break;
	}
	return R1_Response;
}

/*!
 --------------------------------------------------------------------
 \brief Сброс и переключение карты в режим SPI
 
 \return	при успешном выполнении - возвращает 0, это значит что
 карта инициализирована в режиме SPI и находится в idle state
 
 --------------------------------------------------------------------
 */
int SD_Reset_To_SPI_Mode(void)
{	
	// 1. Медленноый этап, втупляем пока microsd заработает
	TIM7_Delay(1000);
	
	// И уменьшаем скорость SPI примерно на 300 КГц.
	SPI2_Set_Low_Speed();
	
	// Выдаём 80 тактов и ожидаем.
	SPI2_DMA_Clocking(10);
	while(SPI2_Soft_Slag.bits.RXRDY == 0);
	SPI2_Soft_Slag.bits.RXRDY = 0;

	// Отправляем команду сброса и ожидаем пока карта 
	// ответит или произойдёт таймаут ожидания.
	SDCARD_CS_ON
	SD_Send_Command(CMD0, 0);
	SD_R1_Response = SD_Get_Status(8);
	SDCARD_CS_OFF	
	
	// Смотрим что ответила карта
	// Тут всё хорошо, карта в режиме ожидания
	if (SD_R1_Response == 0x01) return SD_OK; 
	
	// А это таймаут
	if (SD_R1_Response == 0xFF) return SD_TIMEOUT_ERROR; 

	// ну если два первых прошли то смотрим 
	// уже в регистр R1 что нам ответили
	return SD_UNKNOWN_ERROR;
}

/*!
 --------------------------------------------------------------------
 \brief Эта функция получает версию SD-карты.
 
 Если запрос CMD8 возвращает 0x05 (Illegal command) это значит что 
 карта версии 1 если возвращает idle state - это значит карта ver.2
 в этих случаях карта изменит состояние бита SD_Version.
 
 \return	код ошибки
 
 --------------------------------------------------------------------
 */
int SD_Get_Version(void)
{
	// Тип SD карты
	SDCARD_CS_ON
	
	SD_Send_Command(CMD8, 0x01AA);
	SD_R1_Response = SD_Get_Status(100);
	
	if (SD_R1_Response == 0x01) {	// Прилетел нормальный ответ от SD
		// Вычитываем регистр R7 и проверяем его значение
		SPI2_DMA_Rx_Data((unsigned char *)(&SD_R7_Response), 4);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		SDCARD_CS_OFF
		
		// Проверяем ответ и если ответ не совпал
		// Значит не совместима по напряжениям.
		if (SD_R7_Response != 0xAA010000) return SD_BAD_VOLTAGE;
		
		// И предварительно указываем что версия 2
		SD_CardInfo.CardType = 2;
		return 0;	
	} 
	
	SDCARD_CS_OFF
	
	// Если ответила не 0x05 значит смотрим позже что ответила
	if (SD_R1_Response != 0x05) return SD_UNKNOWN_ERROR;
	
	// Иначе SD-карты версии 1
	SD_CardInfo.CardType = 1;
	
	return 0;	
	
}

/*!
 --------------------------------------------------------------------
 \brief Эта функция вычитывает регистр OCR.
 
 Это нужно для того, что бы узнать информацию о питании SD_карты
 и главное - это тип карты SDSD, SDHC или SDXC.
 
 \return код ошибки
 
 --------------------------------------------------------------------
 */
int SD_Read_OCR_Reg(void)
{
	SDCARD_CS_ON

	SD_Send_Command(CMD58, 0);
	SD_R1_Response = SD_Get_Status(100);	
	
	if (SD_R1_Response != 0xFF) {
		SPI2_DMA_Rx_Data((unsigned char *)(&SD_OCR_Register), 4);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		SDCARD_CS_OFF
		return SD_OK;
	}
	
	SDCARD_CS_OFF
	return SD_TIMEOUT_ERROR;		
}

/*!
 --------------------------------------------------------------------
 \brief Инициализация карты. Вывод из режима Idle
 
 \return	код ошибки. 
 
 --------------------------------------------------------------------
 */
int SD_Init(void)
{
	unsigned int arg;

	SDCARD_CS_ON
	
	SD_Send_Command(CMD55, 0);
	SD_R1_Response = SD_Get_Status(100);
	
	if (SD_R1_Response == 0xFF) {
		SDCARD_CS_OFF
		return SD_UNKNOWN_ERROR;
	}
	
	if (SD_CardInfo.CardType == 1) arg = 0;
	else if (SD_CardInfo.CardType == 2) arg = 0x40000000;
	
	SD_Send_Command(ACMD41, arg);
	SD_R1_Response = SD_Get_Status(10000);
	SDCARD_CS_OFF
	
	if (SD_R1_Response == 0) return SD_OK;
	
	return SD_NOT_INITIALIZED;
}

/*!
 --------------------------------------------------------------------
 \brief Рабор полученных данных из регистров CID и CSD
 
 \param	cardinfo - указатель на инф.структурк SD-карты
 
 --------------------------------------------------------------------
 */
void SD_Read_Info(SD_Info *cardinfo)
{

	cardinfo->SD_csd.CSDStruct = (SD_CSD_Register[1] & 0xC0) >> 6;
	cardinfo->SD_csd.SysSpecVersion = (SD_CSD_Register[1] & 0x3C) >> 2;
	cardinfo->SD_csd.Reserved1 = SD_CSD_Register[1] & 0x03;
	cardinfo->SD_csd.TAAC = SD_CSD_Register[2];
	cardinfo->SD_csd.NSAC = SD_CSD_Register[3];
	cardinfo->SD_csd.MaxBusClkFrec = SD_CSD_Register[4];
	cardinfo->SD_csd.CardComdClasses = (SD_CSD_Register[5] << 4) | 
		(SD_CSD_Register[6] & 0xF0) >> 4;
	cardinfo->SD_csd.RdBlockLen = SD_CSD_Register[6] & 0x0F;
	cardinfo->SD_csd.PartBlockRead = (SD_CSD_Register[7] & 0x80) >> 7;
	cardinfo->SD_csd.WrBlockMisalign = (SD_CSD_Register[7] & 0x40) >> 6;
	cardinfo->SD_csd.RdBlockMisalign = (SD_CSD_Register[7] & 0x20) >> 5;
	cardinfo->SD_csd.DSRImpl = (SD_CSD_Register[7] & 0x10) >> 4;
	cardinfo->SD_csd.Reserved2 = 0;
	
	
	if (cardinfo->CardType < 3) {
		cardinfo->SD_csd.DeviceSize = 
			(((unsigned int)SD_CSD_Register[7] & 0x03) << 8) |
			(((unsigned int)SD_CSD_Register[8]) << 2) |
			(((unsigned int)SD_CSD_Register[9]) >> 6);
			
		cardinfo->SD_csd.MaxRdCurrentVDDMax = SD_CSD_Register[9] & 0x38 >> 3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax = SD_CSD_Register[9] & 0x07;
		cardinfo->SD_csd.MaxWrCurrentVDDMin = (SD_CSD_Register[10] & 0xE0) >> 5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax = (SD_CSD_Register[10] & 0x1C) >> 2;	
		cardinfo->SD_csd.DeviceSizeMul = 
			((SD_CSD_Register[10] & 0x03) << 1) | 
			((SD_CSD_Register[11] & 0x80) >> 7);
			
		cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
		cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) * 
			(1 << (cardinfo->SD_csd.DeviceSizeMul + 2)) * cardinfo->CardBlockSize;
	} else {
		cardinfo->SD_csd.DeviceSize = (SD_CSD_Register[8] & 0x3F) << 16;
		cardinfo->SD_csd.DeviceSize |= (SD_CSD_Register[9]) << 8;
		cardinfo->SD_csd.DeviceSize |= SD_CSD_Register[10];
		cardinfo->CardCapacity = ((unsigned long)cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
		cardinfo->CardBlockSize = 512;
	}
	
	cardinfo->SD_csd.EraseGrSize = (SD_CSD_Register[11] & 0x40) >> 6;
	cardinfo->SD_csd.EraseGrMul = (SD_CSD_Register[11] & 0x3F) << 1;
	cardinfo->SD_csd.EraseGrMul |= (SD_CSD_Register[12] & 0x80) >> 7;
	cardinfo->SD_csd.WrProtectGrSize = (SD_CSD_Register[12] & 0x7F);
	cardinfo->SD_csd.WrProtectGrEnable = (SD_CSD_Register[13] & 0x80) >> 7;
	cardinfo->SD_csd.ManDeflECC = (SD_CSD_Register[13] & 0x60) >> 5;
	cardinfo->SD_csd.WrSpeedFact = (SD_CSD_Register[13] & 0x1C) >> 2;
	cardinfo->SD_csd.MaxWrBlockLen = (SD_CSD_Register[13] & 0x03) << 2;
	
	cardinfo->SD_csd.MaxWrBlockLen |= (SD_CSD_Register[14] & 0xC0) >> 6;
	cardinfo->SD_csd.WriteBlockPaPartial = (SD_CSD_Register[14] & 0x20) >> 5;
	cardinfo->SD_csd.Reserved3 = 0;
	cardinfo->SD_csd.ContentProtectAppli = (SD_CSD_Register[14] & 0x01);
	
	cardinfo->SD_csd.FileFormatGrouop = (SD_CSD_Register[15] & 0x80) >> 7;
	cardinfo->SD_csd.CopyFlag = (SD_CSD_Register[15] & 0x40) >> 6;
	cardinfo->SD_csd.PermWrProtect = (SD_CSD_Register[15] & 0x20) >> 5;
	cardinfo->SD_csd.TempWrProtect = (SD_CSD_Register[15] & 0x10) >> 4;
	cardinfo->SD_csd.FileFormat = (SD_CSD_Register[15] & 0x0C) >> 2;
	cardinfo->SD_csd.ECC = (SD_CSD_Register[15] & 0x03);
	
	cardinfo->SD_csd.CSD_CRC = (SD_CSD_Register[16] & 0xFE) >> 1;
	cardinfo->SD_csd.Reserved4 = 1;
	
	cardinfo->SD_cid.ManufacturerID = SD_CID_Register[1];
	cardinfo->SD_cid.OEM_AppliID = SD_CID_Register[2] >> 8;
	cardinfo->SD_cid.OEM_AppliID |= SD_CID_Register[3];
	
	cardinfo->SD_cid.ProdName1 = SD_CID_Register[4] << 24;
	cardinfo->SD_cid.ProdName1 |= SD_CID_Register[5] << 16;
	cardinfo->SD_cid.ProdName1 |= SD_CID_Register[6] << 8;
	cardinfo->SD_cid.ProdName1 |= SD_CID_Register[7];
	
	cardinfo->SD_cid.ProdName2 = SD_CID_Register[8];
	
	cardinfo->SD_cid.ProdRev = SD_CID_Register[9];
	
	cardinfo->SD_cid.ProdSN = SD_CID_Register[10] << 24;
	cardinfo->SD_cid.ProdSN = SD_CID_Register[11] << 16;
	cardinfo->SD_cid.ProdSN = SD_CID_Register[12] << 8;
	cardinfo->SD_cid.ProdSN = SD_CID_Register[13];
	
	cardinfo->SD_cid.Reserved1 |= (SD_CID_Register[14] & 0xF0) >> 4;
	cardinfo->SD_cid.ManufactDate = (SD_CID_Register[14] & 0x0F) << 8;
	cardinfo->SD_cid.ManufactDate |= SD_CID_Register[15];
	
	cardinfo->SD_cid.CID_CRC = (SD_CID_Register[16] & 0xFE) >> 1;
	cardinfo->SD_cid.Reserved2 = 1;
}

/*!
 --------------------------------------------------------------------
 \brief Получание данных из регистров SID или CSD
 
 \param	cmd - номер команды (CMD9 или CMD10)
 \param	reg - указатель на регистр CID или CSD
 
 --------------------------------------------------------------------
 */
int SD_Get_Reg(unsigned char cmd, unsigned char *reg)
{
	SDCARD_CS_ON
	
	SD_Send_Command(cmd, 0);
	SD_R1_Response = SD_Get_Status(100);
	
	if (SD_R1_Response == 0x00) {
		SPI2_DMA_Rx_Data(reg, 17);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		SDCARD_CS_OFF
		return SD_OK;
	}
	
	SDCARD_CS_OFF
	
	if (SD_R1_Response == 0xFF)
		return SD_TIMEOUT_ERROR;
	
	return SD_UNKNOWN_ERROR;	
}

/*!
 --------------------------------------------------------------------
 \brief Получение информации об SD-карте
 
 --------------------------------------------------------------------
 */
int SD_Get_Info(void)
{
	SD_ErrorCode = SD_Get_Reg(CMD9, SD_CSD_Register);
	if (SD_ErrorCode != SD_OK) return SD_ErrorCode;
	SD_ErrorCode = SD_Get_Reg(CMD10, SD_CID_Register);
	if (SD_ErrorCode != SD_OK) return SD_ErrorCode;
	
	SD_Read_Info(&SD_CardInfo);
	
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief Глобальная инициализация SD-карты
 
 --------------------------------------------------------------------
 */
int SD_Card_Init(void)
{
	int i;
	
	LED1_ON
	
	// Сброс и инициализация SD карты в режим SPI
	SD_ErrorCode = SD_Reset_To_SPI_Mode();
	if(SD_ErrorCode != SD_OK) {
		LED1_OFF
		return SD_ErrorCode; // Выход с ошибкой на этапе инициализации
	}
	
	// Проверим напряжения питания и узнаем версию SD-карты
	SD_ErrorCode = SD_Get_Version();
	if (SD_ErrorCode != SD_OK) {
		LED1_OFF
		return SD_ErrorCode; // Выход с ошибкой на этапе проверки версии		
	}

	// Проверяем биты в регистре OCR
	SD_ErrorCode = SD_Read_OCR_Reg();
	if (SD_ErrorCode != SD_OK) {
		LED1_OFF
		return SD_ErrorCode;
	}
	
	// Проверим ответ от чтения регистра OCR
	if (SD_R1_Response != 0x01) {
		SD_ErrorCode = SD_READ_OCR_ERROR;
		LED1_OFF
		return SD_ErrorCode;
	}
	
	// Проверим напряжения
	if ((SD_OCR_Register & 0x0080FF00) == 0) {
		SD_ErrorCode = SD_CHECK_OCR_ERROR;
		LED1_OFF
		return SD_ErrorCode;
	}
	
	// Запустим процесс инициализации карты
	i = 1000;
	while (i--) {
		SD_ErrorCode = SD_Init();
		if (SD_ErrorCode != SD_NOT_INITIALIZED) break;
	}
	
	if (i <= 0) {
		SD_ErrorCode = SD_INIT_TIMEOUT;
		LED1_OFF
		return SD_INIT_TIMEOUT;
	}
	
	if (SD_ErrorCode != SD_OK) {
		LED1_OFF
		return SD_ErrorCode;
	}
	
	if (SD_CardInfo.CardType == 2) {
		// Если всё же карта оказалась версии 2, то поехали инициализировать дальше
		// Проверяем биты в регистре OCR
		SD_ErrorCode = SD_Read_OCR_Reg();
		if (SD_ErrorCode != SD_OK) {
			LED1_OFF
			return SD_ErrorCode;
		}
		
		// Проверим ответ от чтения регистра OCR
		if (SD_R1_Response != 0x00) {
			LED1_OFF
			SD_ErrorCode = SD_READ_OCR_ERROR;
			return SD_ErrorCode;
		}
		
		if ((SD_OCR_Register & 0x00000080) == 0) {
			LED1_OFF
			SD_ErrorCode = SD_CHECK_OCR_ERROR;
			return SD_ErrorCode;
		}
		
		if (SD_OCR_Register & 0x00000040) {
			SD_CardInfo.CardType = 3;
		}		
	}
	
	if (SD_CardInfo.CardType < 3) {
		SD_ErrorCode = SD_WRONG_VERSION;
		LED1_OFF
		return SD_WRONG_VERSION;
//		SD_ErrorCode = SD_Set_Block_Len();
//		if(SD_ErrorCode != SD_OK) {
//			LED1_OFF
//			return 11;
//		}
	}
	
	SD_ErrorCode = SD_Get_Info();
	if (SD_ErrorCode != SD_OK) {
		LED1_OFF
		return SD_ErrorCode;
	}
	
	LED1_OFF
	SD_ErrorCode = 0;
	SPI2_Set_High_Speed();
	return SD_ErrorCode;
	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int SD_Read_Single_Block(unsigned int sector_addr, unsigned char * block)
{
	unsigned short crc;
	
	LED1_ON
	SDCARD_CS_ON
	
	SD_Send_Command(CMD17, sector_addr);

	SD_R1_Response = SD_Get_Status(10000);
	
	if (SD_R1_Response != 0x00) {
		SDCARD_CS_OFF
		LED1_OFF
		return SD_TIMEOUT_ERROR;
	}
	
	SD_R1_Response = SD_Get_Status(10000);
	
	if (SD_R1_Response == 0xFE) {
		SPI2_DMA_Rx_Data(block, 512);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		
		SPI2_DMA_Rx_Data((unsigned char *)(&crc), 2);
		while(SPI2_Soft_Slag.bits.RXRDY == 0);
		SPI2_Soft_Slag.bits.RXRDY = 0;
		
		SDCARD_CS_OFF
		LED1_OFF
		return SD_OK;
	}
	
	SDCARD_CS_OFF
	LED1_OFF
	return SD_READ_ERROR;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int SD_Read_Multi_Blocks(unsigned int sector_addr, 
	unsigned char * block, unsigned char count)
{
	while (count --) {
		SD_ErrorCode = SD_Read_Single_Block(sector_addr, block);
		if (SD_ErrorCode != SD_OK) break;
		sector_addr++;
		block += 512;
	}
	return SD_ErrorCode;
}
