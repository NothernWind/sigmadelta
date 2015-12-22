/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
/*
---------------------------------------------------------------------
Нужно организовать работу над ошибками
И сделано это будет следующим образом:
Некоторе функции возвращают код ошибки, и если такая имеет место
то специально для этого нужно сделать глобальный обработчик ошибок.
ошибки могут быть следующие
Ошибки в работе периферии I2C и I2S
Ошибки в работе железа, SD-карта, ЦАП CS43L22
Ошибки в работе программ, FATSF, HELIX, MP3, WAV
соответственно при таких ошибках программа должна войти в функцию
обработки ошибок и выдать сообщение типа "нажмите любую клавишу"

А теперь по порядку о том какие могут быть ошибки.
для начала раздеим их на по типу

Information - Будет выводится информационное сообщение.
Для этого в будущей структуре создадим отдельную переменную
в которой будем указывать номер сообщения.

Warning - Предупреждение

Critical error - критическая ошибка

Теперь распишем где какие могу быть ошибки:

1. Проблемы в работе I2C
   I2C_START_ERROR     -1 Ошибка отправки стартового бита
   I2C_DEV_ADDR_ERROR  -2 Ошибка отправки адреса устройства
   I2C_POINTER_ERROR   -3 Ошибка отправки адреса регистра
   I2C_SEND_DATA_ERROR -4 Ошибка отправки данных
   I2C_RSTART_ERROR    -5 Ошибка отправки повторного старта
   I2C_RADDR_ERROR     -6 Ошибка отправки адреса на чтение
   I2C_RCV_DATA_ERROR  -7 Ошибка приёма данных
   I2C_UNKNOWN_ERROR   -9 Неизвесная ошибка

2. Проблемы I2S, в нём может быть только одна проблема - это неверный 
   переданный битрейт.
   
3. проблемы SD-карты
   SD_TIMEOUT_ERROR   1  Таймаут при обращении к SD карте 
   SD_BAD_VOLTAGE     2  Напряжение питания карты не соответствует
   SD_NOT_INITIALIZED 3  Карта не инициализирована
   SD_READ_ERROR      4  Ошибка чтения карты
   SD_UNKNOWN_ERROR   20 Неизвесная ошибка

   Проблемы которые могут быть на этапе инициализации карты
    1 Ошибка на этапе сброса в режим SPI
    2 Ошибка на этапе проверки версии карты
    3 Ошибка на этапе первом этапе чтения регистра OCR
    4 Ошибка проверки регистра OCR не верное значение регистра
    5 Ошибка при проверке напряжения питания
    6 Таймаут при попытке инициализациии
    7 Ошибка при инициализации карты
    8 Ошибка на этапе проверки регистра OCR после инициализации карты
    9 Тоже самое только в этом случае не верное значение R1 Response
   10 Тоже самое, только не верное значение в регистре OCR
   11 Такой тип карты не поддерживается
   12 Ошибка на этапе чтения CSD регистра
   13 Ошибка на этапе чтения CID регистра
   
4. Проблемы которые могут быть с FATFS 
   Все эти проблемы были описаны в файле ff.h
	FR_DISK_ERR,			 1 A hard error occurred in the low level disk I/O layer
	FR_INT_ERR,				 2 Assertion failed
	FR_NOT_READY,			 3 The physical drive cannot work
	FR_NO_FILE,				 4 Could not find the file
	FR_NO_PATH,				 5 Could not find the path
	FR_INVALID_NAME,		 6 The path name format is invalid
	FR_DENIED,				 7 Access denied due to prohibited access or directory full
	FR_EXIST,				 8 Access denied due to prohibited access
	FR_INVALID_OBJECT,		 9 The file/directory object is invalid
	FR_WRITE_PROTECTED,		10 The physical drive is write protected
	FR_INVALID_DRIVE,		11 The logical drive number is invalid
	FR_NOT_ENABLED,			12 The volume has no work area
	FR_NO_FILESYSTEM,		13 There is no valid FAT volume
	FR_MKFS_ABORTED,		14 The f_mkfs() aborted due to any parameter error
	FR_TIMEOUT,				15 Could not get a grant to access the volume within defined period
	FR_LOCKED,				16 The operation is rejected according to the file sharing policy
	FR_NOT_ENOUGH_CORE,		17 LFN working buffer could not be allocated
	FR_TOO_MANY_OPEN_FILES,	18 Number of open files > _FS_SHARE
	FR_INVALID_PARAMETER	19 Given parameter is invalid
	
5. Проблемы которые могут возникнуть при инициализации звукового
   ЦАП CS43L22. С ним тоже будет всё просто. две ошибки, ошибка вызванная
   проблемами на шине I2C и проблемы при инициализации I2S.
   
6. Проблемы которые могу возникнуть при воспроизведении WAV.
   т.к. с этот пункт мы будем ещё переделывать, то пока опустим этот вопрос
   проблемы при воспроизведении WAV могу быть следующие:
   - Аппаратные
   - Ошибка файловой системы.

7. Проблемы МР3:
   - Проблема при чтении файла.
   - Проблема при инициализации кодека
   - Проблемы при декодировании
   - Аппаратные проблемы

8. Файловые манагер:
   - Ошибки файловой системы
   - Аппаратные проблемы
   - Для этого случая ещё будем выводить сообщение в том случае если 
     карта памяти была извлечена.
   
---------------------------------------------------------------------
*/
#include "msg.h"

#include <stdio.h>

//int MSG_Error_ID;	// Посути тут нужно чказать где произошла ошибка
/*
MSG_FF_ERROR	// Ошибка в файловой системе
MSG_MP3_ERROR	// Ошибка в MP3
MSG_WAV_ERROR	// Ошибка в WAV
*/
//unsigned int MSG_FF_Error;	// Коды ошибок fatfs
//int MSG_MessageType;
char MSG_HW_Error;

//extern t_msg msg;
//char str[40];
char MSG_Name[32];
char MSG_Sw_Str[80];
char MSG_Hw_Str[80];
char MSG_Header[10];

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_print_border(int msg_type)
{
	int i;
	diplay_fgcolor.c = 0xFFFF;
	display_bgcolor.c = 0;
	
	display_print_char(0, 0, 0xD5);
	display_print_char(39, 0, 0xB8);
	display_print_char(39, 19, 0xD9);
	display_print_char(0, 19, 0xC0);
	
	for (i = 0; i < 38; i++) {
		display_print_char(i + 1, 0, 0xDB);
		display_print_char(i + 1, 19, 0xC4);
	}
	
	for (i = 0; i < 18; i++) {
		display_print_char(0, i + 1, 0xB3);
		display_print_char(39, i + 1, 0xB3);
	}
	
	switch (msg_type) {
		case MSG_INFORMATION:
			sprintf(MSG_Header, "Information");
			display_print_icon_32x32(16, 26, info_icon);
			break;
		case MSG_WARNING:
			sprintf(MSG_Header, "Warning");
			display_print_icon_32x32(16, 26, warning_icon);
			break;
		case MSG_CRITICAL:
			sprintf(MSG_Header, "Error");
			display_print_icon_32x32(16, 26, error_icon);
			break;
		default: break;
	}
	
	diplay_fgcolor.c = 0;
	display_bgcolor.c = 0xFFFF;
	display_print_text(2, 0, MSG_Header);

	diplay_fgcolor.c = 0xFFFF;
	display_bgcolor.c = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_sd_ejected(void)
{
	display_clear();
	msg_print_border(MSG_INFORMATION);
	sprintf(MSG_Header, "SD-Card ejected");
	display_print_text(8, 3, MSG_Header);
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_sd_error(void)
{
	int no_error = 0;
	
	switch(SD_ErrorCode) {
		case SD_TIMEOUT_ERROR:
			sprintf(MSG_Hw_Str, 
				"SD_TIMEOUT_ERROR:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		case SD_BAD_VOLTAGE:
			sprintf(MSG_Hw_Str, 
				"SD_TIMEOUT_ERROR:\n"
				"R7 Response: 0x%08X", SD_R7_Response);
			break;
		case SD_NOT_INITIALIZED:
			sprintf(MSG_Hw_Str, 
				"SD_NOT_INITIALIZED:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		case SD_READ_ERROR:
			sprintf(MSG_Hw_Str, 
				"SD_READ_ERROR:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		case SD_READ_OCR_ERROR:
			sprintf(MSG_Hw_Str, 
				"SD_READ_OCR_ERROR:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		case SD_CHECK_OCR_ERROR:
			sprintf(MSG_Hw_Str, 
				"SD_CHECK_OCR_ERROR:\n"
				"R7 Response: 0x%08X", SD_OCR_Register);
			break;
		case SD_INIT_TIMEOUT:
			sprintf(MSG_Hw_Str, 
				"SD_INIT_TIMEOUT:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		case SD_WRONG_VERSION:
			sprintf(MSG_Hw_Str, "Wrong SDC version");
			break;
		case SD_UNKNOWN_ERROR:
			sprintf(MSG_Hw_Str, 
				"SD_UNKNOWN_ERROR:\n"
				"R1 Response: 0x%02X", SD_R1_Response);
			break;
		default:
			no_error = 1;
			break;
	}
	if (no_error == 1) return;
	
	display_print_text(2, 9, MSG_Hw_Str);	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_ff_error(int error_code)
{	
	switch(error_code) {
		case FR_DISK_ERR:
			MSG_HW_Error = 1;
			sprintf(MSG_Sw_Str, 
				"A hard error occurred in the low\n"
				"level disk I/O layer");
			break;
		case FR_INT_ERR:
			sprintf(MSG_Sw_Str, "Assertion failed");
			break;
		case FR_NOT_READY:
			MSG_HW_Error = 1;
			sprintf(MSG_Sw_Str, "The physical drive cannot work");
			break;
		case FR_NO_FILE:
			sprintf(MSG_Sw_Str, "Could not find the file");
			break;
		case FR_NO_PATH:
			sprintf(MSG_Sw_Str, "Could not find the path");
			break;
		case FR_INVALID_NAME:
			sprintf(MSG_Sw_Str, "The path name format is invalid");
			break;
		case FR_DENIED:
			sprintf(MSG_Sw_Str, 
				"Access denied due to prohibited\n"
				"access or directory full");
			break;
		case FR_EXIST:
			sprintf(MSG_Sw_Str, 
				"Access denied due to prohibited\n"
				"access");
			break;
		case FR_INVALID_OBJECT:
			sprintf(MSG_Sw_Str, 
				"The file/directory object is\n"
				"invalid");
			break;
		case FR_WRITE_PROTECTED:
			sprintf(MSG_Sw_Str, 
				"The physical drive is write\n"
				"protected");
			break;
		case FR_INVALID_DRIVE:
			sprintf(MSG_Sw_Str, "The logical drive number is invalid");
			break;
		case FR_NOT_ENABLED:
			sprintf(MSG_Sw_Str, "The volume has no work area");
			break;
		case FR_NO_FILESYSTEM:
			sprintf(MSG_Sw_Str, "There is no valid FAT volume");
			break;
		case FR_MKFS_ABORTED:
			sprintf(MSG_Sw_Str, 
				"The f_mkfs() aborted due to any\n"
				"parameter error");
			break;
		case FR_TIMEOUT:
			sprintf(MSG_Sw_Str, 
				"Could not get a grant to access the\n"
				"volume within defined period");
			break;
		case FR_LOCKED:
			sprintf(MSG_Sw_Str, 
				"The operation is rejected according\n"
				"to the file sharing policy");
			break;
		case FR_NOT_ENOUGH_CORE:
			sprintf(MSG_Sw_Str, 
				"LFN working buffer could not be\n"
				"allocated");
			break;
		case FR_TOO_MANY_OPEN_FILES:
			sprintf(MSG_Sw_Str, "Number of open files > _FS_SHARE");
			break;
		case FR_INVALID_PARAMETER:
			sprintf(MSG_Sw_Str, "Given parameter is invalid");
			break;
		default:break;
	}
	
	display_clear();
	
	msg_print_border(MSG_CRITICAL);
	
	sprintf(MSG_Name, "FAT Error");
	display_print_text(8, 3, MSG_Name);	
	
	display_print_text(2, 6, MSG_Sw_Str);	
	
	
	if (MSG_HW_Error) {
		msg_sd_error();
	}
	
	while (exit_button.pressed == 0);
	exit_button.pressed = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_mp3_warning(void)
{
	display_clear();
	msg_print_border(MSG_WARNING);
	sprintf(MSG_Name, "MP3 Decoder warning");
	display_print_text(8, 3, MSG_Name);	
	sprintf(MSG_Sw_Str, "Wrong MP3 File");
	display_print_text(2, 6, MSG_Sw_Str);
	
	while (exit_button.pressed == 0);
	exit_button.pressed = 0;	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_i2c_error(void)
{
	display_clear();
	msg_print_border(MSG_CRITICAL);
	
	sprintf(MSG_Name, "I2C Bus");
	display_print_text(8, 3, MSG_Name);
	
	sprintf(MSG_Hw_Str, "I2C Error: %d", I2C_Status);
	display_print_text(2, 6, MSG_Hw_Str);
	
	while (exit_button.pressed == 0);
	exit_button.pressed = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_i2s_error(void)
{
	display_clear();
	msg_print_border(MSG_CRITICAL);
	
	sprintf(MSG_Name, "I2S init error");
	display_print_text(8, 3, MSG_Name);
	
	sprintf(MSG_Hw_Str, "Wrong sample rate");
	display_print_text(2, 6, MSG_Hw_Str);
	
	while (exit_button.pressed == 0);
	exit_button.pressed = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_adio_codec_error(int error_code)
{
	if (error_code == -1) {
		msg_i2c_error();
	} else if (error_code == -2) {
		msg_i2s_error();
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_mp3_unk_error(int error_code)
{
	display_clear();
	msg_print_border(MSG_WARNING);
	
	sprintf(MSG_Name, "MP3 Warning");
	display_print_text(8, 3, MSG_Name);
	
	sprintf(MSG_Hw_Str, "I have no idea: %d", error_code);
	display_print_text(2, 6, MSG_Hw_Str);
	
	while (exit_button.pressed == 0);
	exit_button.pressed = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void msg_travel(void)
{
	int x, y;
	t_color clr;
	
	display_clear();
	msg_print_border(MSG_INFORMATION);
	sprintf(MSG_Header, "Travel!");
	display_print_text(8, 3, MSG_Header);
	
	sprintf(MSG_Sw_Str, "Travel. As much as you can.");
	display_print_text(2, 6, MSG_Sw_Str);
	sprintf(MSG_Sw_Str, "As far as you can.");
	display_print_text(2, 7, MSG_Sw_Str);
	sprintf(MSG_Sw_Str, "As long as you can.");
	display_print_text(2, 8, MSG_Sw_Str);
	sprintf(MSG_Sw_Str, "Life\'s not meant to be lived");
	display_print_text(2, 9, MSG_Sw_Str);
	sprintf(MSG_Sw_Str, "in one place.");
	display_print_text(2, 10, MSG_Sw_Str);
	
	for (x = 0; x < 18; x++) {
		for (y = 0; y < 5; y++) {
			clr.rgb.red = (x % 2) * 4 - 1;
			clr.rgb.green = (x + y) * 4 - 1;
			clr.rgb.blue = (y % 2) * 4 - 1;
			draw_rect((x * 16) + 16, (y * 16) + 140, 15, 15, clr.c);
		}
	}
}
