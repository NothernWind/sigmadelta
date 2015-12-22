/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "mp3.h"

// Prototypes -------------------------------------------------------
void MP3_Exit(void);
int MP3_Init(const char * mp3_name);
int MP3_Buffering(void);
int MP3_Prepare_Audio(void);

// Variables --------------------------------------------------------
FIL MP3_File;			// MP3 файл
FILINFO MP3_File_Info;	// И информация о нём

char MP3_File_Buf[MP3_FBUF_SIZE]; // Буфер файла MP3
int MP3_File_Size;		// Размер MP3 файла
int MP3_File_Offset;	// Позиция в файле

HMP3Decoder MP3_Decoder;		//
MP3FrameInfo MP3_Frame_Info;	// Информация о кадре 

unsigned short MP3_Out_Buf[2][MP3_OBUF_SIZE]; // Буфер декодированных данных
unsigned short MP3_Temp_Out[MP3_OBUF_SIZE];

char *MP3_Read_Ptr;		// Указатель на буфер декодирования

unsigned int MP3_Bytes_To_Read;	// Сколько нужно прочитать из файла
unsigned int MP3_Bytes_Read;	// Сколько было прочитано из файла

int MP3_BytesLeft;		// Сколько байт осталось не декодированными
int MP3_Prev_BytesLeft;	// 

int MP3_Offset;			// Смещение до начала кадра
int MP3_OutBuf_Size;	// Размер выходных данных

int MP3_Buf_BytesLeft;	// Остаток в буфере

// Флаги
char MP3_File_End;		// 0 - в файле ещё есть данные
char MP3_nBuf;			// Номер буфера для двойной буферизации
char MP3_CS43L22_Status;	// Статус ЦАП. 0 - не инициализирован
char MP3_Stop;			// Воспроизведение MP3 закончено

// Ошибки
int MP3_Error;	// Тип ошибки
unsigned int MP3_FsError;	// Код ошибки файловой системы
int MP3_DecError;			// Код ошибки Декодера
int MP3_Hw_Error;			// Код ошибки железа

int MP3_Start_From;
char MP3_Is_Start;
int MP3_Pass_Frame;

int MP3_Time;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int MP3_Init(const char * mp3_name)
{
	MP3_File_Size = 0;
	MP3_File_Offset = 0;
	MP3_File_End = 0;
	MP3_BytesLeft = 0;
	MP3_Offset = 0;
	MP3_OutBuf_Size = 0;
	MP3_Error = 0;
	MP3_Bytes_To_Read = 0;
	MP3_Bytes_Read = 0;
	MP3_nBuf = 0;
	MP3_CS43L22_Status = 0;
	MP3_Buf_BytesLeft = 0;
	MP3_Prev_BytesLeft = 0;
	MP3_FsError = 0;
	MP3_DecError = 0;
	MP3_Hw_Error = 0;
	
	MP3_Start_From = 0;
	MP3_Is_Start = 0;
	MP3_Pass_Frame = 0;
	
	MP3_FsError = f_open(&MP3_File, mp3_name, FA_READ);
	if (MP3_FsError != FR_OK) return MP3_FS_ERROR;
	
	// Получить информацию о файле
	MP3_FsError = f_stat(mp3_name, &MP3_File_Info);
	if (MP3_FsError != FR_OK) {
		f_close(&MP3_File);
		return MP3_FS_ERROR;
	}
	MP3_File_Size = MP3_File_Info.fsize;
	
	MP3_Decoder = MP3InitDecoder();
	if (MP3_Decoder == 0) return MP3_CODEC_ERROR;
	
	MP3_BytesLeft = MP3_File_Size;
	MP3_Offset = -1;
	
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int MP3_Buffering(void)
{
	// Если в фале больше нечего читать - просто выодим.
	if (MP3_File_End == 1) return 0;
	
	// Сначала нам нужно понять сколько байт нам нужно вычитать
	// Если буфер нужно заполнить заново полностью, то перед этим 
	// переменную MP3_Buf_BytesLeft нужно тупо обнулить
	MP3_Bytes_To_Read = MP3_FBUF_SIZE - MP3_Buf_BytesLeft;
	
	// Теперь нужно проверить можем ли мы вычитаь столько байт из буфера
	if (MP3_File_Offset + MP3_Bytes_To_Read > MP3_File_Size) {
		// Если мы не можем вчитать то количество байт, то
		// то узнаём сколько мы можем вычиать байт.
		MP3_Bytes_To_Read = MP3_File_Size - MP3_File_Offset;
		
		// Это кстати значит ещё и то, что файл закончился
		// А значит нужно выставить соответствующий флаг.
		MP3_File_End = 1;
	}
	
	// Теперь нам нужно перезаписать все оставшиеся данные в буфере
	// в его начало. Но перед этим нужно задаться вопросом нужно ли это.
	// Если буфер нужно перезаписать полностью, а об этом будет говорить
	// тот факт, что MP3_Buf_BytesLeft равен нулю. т.е. в буфере нет 
	// полезных данных.
	// Если они всё же есть, то перезаписываем остатки в начало буфера
	if (MP3_Buf_BytesLeft != 0) {
		// т.е. по указателю MP3_Read_Ptr, считываем данные в начало 
		// MP3_File_Buf в количестве MP3_Buf_BytesLeft
		memmove(MP3_File_Buf, MP3_Read_Ptr, MP3_Buf_BytesLeft);
	}
	
	// а вот теперь нам нужно вычитать данные из файла
	// а для этого сначала нужно установить смещение в файле.
	MP3_FsError = f_lseek(&MP3_File, MP3_File_Offset);
	if (MP3_FsError != FR_OK) return MP3_FS_ERROR;
	
	// Ну а теперь нужно вычитать данные из файла
	MP3_FsError = f_read(&MP3_File, (unsigned char *)(&MP3_File_Buf[MP3_Buf_BytesLeft]), 
		MP3_Bytes_To_Read, &MP3_Bytes_Read);
	if (MP3_FsError != FR_OK) return MP3_FS_ERROR;
		
	// Данные успешно вычитали, теперь нужно обновить переменные
	
	// Новое количество байт в буфере
	MP3_Buf_BytesLeft += MP3_Bytes_To_Read;
	
	// Указатель на MP3 буфер
	MP3_Read_Ptr = MP3_File_Buf;
	
	// И за одно смещение в файле для последующего чтения
	MP3_File_Offset += MP3_Bytes_To_Read;
	
	MP3_Offset = -1;
	
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int MP3_Prepare_Audio(void)
{
	if (MP3_Buffering() != 0) return MP3_FS_ERROR;
	
	MP3_Offset = MP3FindSyncWord((unsigned char*)MP3_Read_Ptr, MP3_Buf_BytesLeft);
	
	if (MP3_Offset < 0) {
		
		// Сначала нужно найти синхронизацию
		while(MP3_Offset < 0 && MP3_File_End == 0) {
			// Так как в этом куске файла не найдено полезных данных
			// Или первый раз читаем этот файл, то заполняем буфер.
			// Обнуляем количество байт в буфере, т.е. как бы выкидывааем
			// нафиг.
			MP3_Buf_BytesLeft = 0;
			
			// Теперь нужно заполнить буфер заново
			if (MP3_Buffering() != 0) return MP3_FS_ERROR;
			
			// и проверить наличие данных в новом пополнении
			MP3_Offset = MP3FindSyncWord((unsigned char*)MP3_Read_Ptr, MP3_Buf_BytesLeft);
		}
	}
	
	// Теперь нужно узнать найдена ли синхронизация
	// И если она не найдена, значит кодеко можно смело останавливать
	// потому что данные в файле закончились.
	if (MP3_Offset < 0) return 1;
	LED3_ON
	// Если нет. то продолжаем декодирование.
	// Сдвинем указатель от куда нужно начинать читать
	MP3_Read_Ptr += MP3_Offset;
	// Уменьшим количество байт в файле
	MP3_BytesLeft -= MP3_Offset;
	// Уменьшим количество байт в буфере
	MP3_Buf_BytesLeft -= MP3_Offset;
	// Запомним предыдущее количество байт
	MP3_Prev_BytesLeft = MP3_BytesLeft;
	
	MP3_DecError = MP3Decode(
		MP3_Decoder, 
		(unsigned char **)&MP3_Read_Ptr, 
		&MP3_BytesLeft, (
		short *)(&MP3_Out_Buf[MP3_nBuf][0]), 
		0);
		
	MP3_Buf_BytesLeft -= (MP3_Prev_BytesLeft - MP3_BytesLeft);
	LED3_OFF
	if (MP3_DecError < 0) {
		MP3_Pass_Frame++;
		if (MP3_DecError < -2) {
			MP3_Read_Ptr += 2;
			MP3_BytesLeft -= 2;
			MP3_Buf_BytesLeft -= 2;	
		}
		return MP3_CODEC_ERROR;
	}
	
	if (MP3_DecError == 0) {
		MP3GetLastFrameInfo(MP3_Decoder, &MP3_Frame_Info);
		if (MP3_Is_Start == 0) {
			MP3_Is_Start = 1;
			MP3_Start_From = MP3_File_Size - MP3_BytesLeft;
		}
		return 0;
	}
	return MP3_UNKNOWN_ERROR;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void MP3_extend_channel(int smps)
{
	int i;
	
	for (i = 0; i < smps; i++) {
		MP3_Temp_Out[i] = MP3_Out_Buf[MP3_nBuf][i];
	}
	
	for (i = 0; i < smps; i++) {
		MP3_Out_Buf[MP3_nBuf][i * 2] = MP3_Temp_Out[i];
		MP3_Out_Buf[MP3_nBuf][i * 2 + 1] = MP3_Temp_Out[i];
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void MP3_Play(const char * mp3_name)
{
	char str[20];
	
	MP3_Error = MP3_Init(mp3_name);
	if (MP3_Error != 0) {
		MP3_Exit();
		return;
	}
	
	MP3_Stop = 0;
	
	display_clear();
	
	diplay_fgcolor.rgb.red = 0x00;
	diplay_fgcolor.rgb.green = 0x3F;
	diplay_fgcolor.rgb.blue = 0x00;
			
	display_bgcolor.c = 0;
	
	printf("%s\n", mp3_name);
						//   "--------------------"
	display_print_text(0, 2, "Size: \0");
	display_print_text(0, 3, "Bytes left:\0");
	display_print_text(0, 4, "Frame Info:\0");
	display_print_text(0, 5, "Bit rate:\0");
	display_print_text(0, 6, "Chanels:\0");
	display_print_text(0, 7, "Sample Rate:\0");
	display_print_text(0, 8, "Bits PerS ample:\0");
	display_print_text(0, 9, "Output samples:\0");
	display_print_text(0, 10, "Layer:\0");
	display_print_text(0, 11, "Version:\0");	
	display_print_text(0, 12, "Start From:\0");
	display_print_text(0, 13, "Bytes Read:\0");
	display_print_text(0, 14, "Skip:\0");
	
	while (1) {
		if (MP3_Stop == 1) break;
		MP3_Error = MP3_Prepare_Audio();
		if (MP3_Error == 1 || MP3_Error == MP3_FS_ERROR) break;
		if (MP3_Error == MP3_CODEC_ERROR && MP3_DecError == -1) break;
			
		diplay_fgcolor.rgb.red = 0x00;
		diplay_fgcolor.rgb.green = 0x00;
		diplay_fgcolor.rgb.blue = 0x1F;
		
		display_bgcolor.c = 0;
		
		MP3_Time = (MP3_BytesLeft) / (MP3_Frame_Info.bitrate / 8);
		
		sprintf(str, "%8d", MP3_File_Size);
		display_print_text(20, 2, str);
		
		sprintf(str, "%8d", MP3_BytesLeft);
		display_print_text(20, 3, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.bitrate);
		display_print_text(20, 5, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.nChans);
		display_print_text(20, 6, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.samprate);
		display_print_text(20, 7, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.bitsPerSample);
		display_print_text(20, 8, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.outputSamps);
		display_print_text(20, 9, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.layer);
		display_print_text(20, 10, str);
		
		sprintf(str, "%d    ", MP3_Frame_Info.version);
		display_print_text(20, 11, str);
		
		sprintf(str, "%d    ", MP3_Start_From);
		display_print_text(20, 12, str);
		
		sprintf(str, "%d    ", MP3_Bytes_Read);
		display_print_text(20, 13, str);
		
		sprintf(str, "%d    ", MP3_Pass_Frame);
		display_print_text(20, 14, str);
		
		sprintf(str, "%02d:%02d    ", MP3_Time / 60, MP3_Time % 60);
		display_print_text(20, 15, str);
		
		

		if (MP3_Error == 0) {
			if (MP3_CS43L22_Status == 0) {
				if ( MP3_Frame_Info.samprate == 0) {
					printf("Bad Sample Rate\n");
					MP3_Error = MP3_BAD_SAMPLE_RATE;
					break; 
				}
				MP3_Hw_Error = CS43L22_Init(MP3_Frame_Info.samprate);
				if (MP3_Hw_Error != 0) {
					MP3_Error = MP3_HARDWARE_ERROR;
					break;
				}
				CS43L22_on();
				MP3_CS43L22_Status = 1;
				I2S_Transfer_Complete = 1;
			}
			
			MP3_OutBuf_Size = MP3_Frame_Info.outputSamps;
			
			if (MP3_Frame_Info.nChans == 1) {
				
				MP3_extend_channel(MP3_OutBuf_Size);
				
				MP3_OutBuf_Size *= 2;
			}
			
			LED4_OFF
			
			while(I2S_Transfer_Complete == 0);			
			I2S_Transfer_Complete = 0;
			I2S_Send_Audio_Buffer((unsigned short *)(&MP3_Out_Buf[MP3_nBuf][0]), MP3_OutBuf_Size);
			
			LED2_TOGGLE
			LED4_ON
			// swap buffer
			if (MP3_nBuf == 1) MP3_nBuf = 0;
			else MP3_nBuf = 1;

			MP3_Frame_Info.bitrate = 0;
			MP3_Frame_Info.nChans = 0;
			MP3_Frame_Info.samprate = 0;
			MP3_Frame_Info.bitsPerSample = 0;
			MP3_Frame_Info.outputSamps = 0;
			MP3_Frame_Info.layer = 0;
			MP3_Frame_Info.version = 0;
		}
		
		CS43L22_VolumeControl();
		
		if (exit_button.pressed == 1) {
			exit_button.pressed = 0;
			while(I2S_Transfer_Complete == 0);
			I2S_Transfer_Complete = 0;
			break;
		}
	}
	MP3_Exit();
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void MP3_Exit(void)
{
	f_close(&MP3_File);
	MP3FreeDecoder(MP3_Decoder);
	if (MP3_CS43L22_Status != 0) {
		CS43L22_off();
		CS43L22_DeInit();
	}
	MP3_CS43L22_Status = 0;
	
	switch(MP3_Error) {
		case 0:
			break;
		case 1:
			while(exit_button.pressed == 0);
			exit_button.pressed = 0;
			break;
		case MP3_FS_ERROR:
			msg_ff_error(MP3_FsError);
			break;
		case MP3_CODEC_ERROR:
			msg_mp3_warning();
			break;
		case MP3_HARDWARE_ERROR:
			msg_adio_codec_error(MP3_Hw_Error);
			break;
		default:
			msg_mp3_unk_error(MP3_Error);
			break;
	}
}
