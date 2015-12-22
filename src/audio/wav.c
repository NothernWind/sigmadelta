/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "wav.h"

// Prototypes -------------------------------------------------------
void WAV_Reset(void);
int WAV_SendFrame(FIL * wavfile, int size);

// Defines ----------------------------------------------------------
#define WAV_FRAME_SIZE	441 // Количество слов(16 бит) в буфере

// Поменять буфер
#define mSwapBuffer(b) b = (b == 1) ? 0 : 1;

// Variables --------------------------------------------------------
int WAV_ErrorCode;

FIL WAV_File;
FILINFO WAV_File_Info;
t_wave_header WAV_Header;

unsigned short wav_db_frame[2][WAV_FRAME_SIZE];
char n_db_frame;	// Номер буфера в двойной буферизации
int frame_index;	// Номер фрейма
int frame_size;

char frame_rdy;
char file_end;
char wav_end;

char wav_fmt_str[16];

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void WAV_Reset(void)
{
	I2S_Transfer_Complete = 1;
	n_db_frame = 0;
	frame_index = 0;
	WAV_ErrorCode = 0;
	frame_size = WAV_FRAME_SIZE;
	frame_rdy = 0;
	file_end = 0;
	wav_end = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int WAV_SendFrame(FIL * wavfile, int size)
{
	unsigned int n;
	// Чтение из файла ----------------------------------------------

	// Если хоть один буфер свободен
	if (frame_rdy < 2) {
		// Установим позицию файла
		
		// Проверим сколько в осталось в файле байтов
		if ((frame_index * WAV_FRAME_SIZE * 2) > size) {
			// Если Байт в файле не хватает на полный кадр меняем размер кадра
			frame_size = ((frame_index * WAV_FRAME_SIZE * 2) - size) * 2;
			// Даём сигнал о том что файл закончился и вычитываются последные байты
			file_end = 1;
		}
		
		// Перемещаемся на позицию в файле
		WAV_ErrorCode = f_lseek(wavfile, (frame_index * WAV_FRAME_SIZE * 2) + 44);
		if (WAV_ErrorCode != FR_OK) return WAV_FILE_ERROR;
		
		// Считывааем фрагмент из фала
		WAV_ErrorCode = f_read(wavfile, &wav_db_frame[n_db_frame][0], frame_size * 2, &n);
		if (WAV_ErrorCode != FR_OK) return WAV_FILE_ERROR;
		
		// Подготовим индекс для следующего чтения
		frame_index++;

		// это нужно что бы понимать сколько буферов мы наполнили
		frame_rdy++;
	}
	// Отправка в ЦАП -----------------------------------------------
	
	// Если переача в цап не завершена, то выходим со статусом занято
	if(I2S_Transfer_Complete == 0) return WAV_I2S_BUSY;
	// ЦАП освободился, сбрасываем флаг
	I2S_Transfer_Complete = 0; 
	
	if (wav_end != 0) return WAV_COMPLETE;
	
	TOGGLE_MARK
	// И указываем ему буфер
	I2S_Send_Audio_Buffer((unsigned short *)(&wav_db_frame[n_db_frame][0]), frame_size);
	
	// Поменять буфер 
	mSwapBuffer(n_db_frame);
	
	if (file_end != 0) {
		wav_end = 1;
		return 0;
	}
		
	// Если этот хитрый индекс больше 1
	if (frame_rdy > 1) frame_rdy = 1;
	
	return WAV_OK;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void WAV_Play(const char * wav_name) 
{
	unsigned int res;
	int wav_stat;
	unsigned int n_bts;
	int min = 0;
	int sec = 0;
	int smps = 0;
	int size;
	float wav_size;
	
	int time;
	char str[40];
	
	res = f_open(&WAV_File, wav_name, FA_READ);
	
	if (res != FR_OK) {
		printf("Can't open file:%s, [0x%04X]\r\n", wav_name, res);
		SD_Get_Error(0, res);
		return;
	}
	
	f_stat(wav_name, &WAV_File_Info);
	
	f_read(&WAV_File, (unsigned char *)(&WAV_Header), 44, &n_bts);
	
	wav_size = WAV_File_Info.fsize;
	
	while (wav_size > 1000) {
		wav_size /= 1024;
		smps++;
	}
	
	printf("\n%s\n", wav_name);
	printf("size: %0.2f " ,wav_size);
	switch(smps) {
		case 0:
			printf("Bytes\n");
			break;
		case 1:
			printf("KB\n");
			break;
		case 2:
			printf("MB\n");
			break;
		case 3:
			printf("GB\n");
			break;
		default:
			printf("\n");
			break;
	}
	
	smps = 0;
	
	printf("\nWave info:\n");
	
	memcpy(wav_fmt_str, (char *)(&WAV_Header.ChunkID), 4);
	wav_fmt_str[4] = ' ';
	memcpy(&wav_fmt_str[5], (char *)(&WAV_Header.RIFFType), 4);
	wav_fmt_str[9] = ' ';
	memcpy(&wav_fmt_str[10], (char *)(&WAV_Header.subchunk1Id), 4);
	wav_fmt_str[14] = 0;
	printf("%s\n",wav_fmt_str);

	time = ((WAV_Header.chunkSize + 8) - 44) / ((WAV_Header.bitsPerSample / 8) * 
		WAV_Header.numChannels * WAV_Header.sampleRate);
	
	sec = time % 60;
	min = time / 60;	
//	printf("Size:        %d \n", WAV_Header.chunkSize);
	printf("Channels:    %d \n", WAV_Header.numChannels);
	printf("Sample rate: %d Hz\n", WAV_Header.sampleRate);
	printf("Bit rate:    %d bps\n", WAV_Header.byteRate / 125);
	printf("Bit depth:   %d bits\n", WAV_Header.bitsPerSample);	
	printf("Duration:    %d min %ds\n \n \n", min, sec);
	
	if (WAV_Header.numChannels != 2 || WAV_Header.bitsPerSample != 16) {
		printf("Bad WAV File\n");
		printf("Press Exit...\n");
		while(exit_button.pressed == 0);
		exit_button.pressed = 0;
		f_close(&WAV_File);
		return;
	}
	
	sec = 0;
	min = 0;

	draw_time(0, 228, min, sec);
	
	CS43L22_Init(WAV_Header.sampleRate);
	CS43L22_on();
	
	size = (WAV_Header.chunkSize + 8) - 44;
	
	WAV_Reset();
	
	while (1) {
		CS43L22_VolumeControl();
		
		if (exit_button.pressed == 1) {
			exit_button.pressed = 0;
				file_end = 1;
		}
		
		wav_stat = WAV_SendFrame(&WAV_File, size);
		
		if (wav_stat == WAV_I2S_BUSY) continue;
		
		if (wav_stat == WAV_OK) {
			smps++;
			if (smps > 199) {
				smps = 0;
				sec++;
				if (sec > 59) {
					sec = 0;
					min++;
					if (min > 59) {
						min = 0;
					}
				}
				
				diplay_fgcolor.c = 0xFFFF;
				display_bgcolor.c = 0;
				sprintf(str, "Elapsed time - %2d:%02d", min, sec);
				display_print_text(0, 19, str);
				//draw_time(0, 228, min, sec);
			}			
			continue;
		}
		
		if (wav_stat == WAV_COMPLETE) {
			printf("play file complete\n");
			break;
		}
		
		if (wav_stat == WAV_FILE_ERROR) {
			SD_Get_Error(0, WAV_ErrorCode);
			break;
		}
	}
	
	f_close(&WAV_File);
	
	CS43L22_off();
	CS43L22_DeInit();
}

/*
---------------------------------------------------------------------
В этом фале нужно сделать совсем не много. просто окрыть файл и впихнуть
функцию воспроизведения. в этой функции должно быть совсем не много
прочитали фрагмент из файла и запихнули его в ЦАП, и всё.

но это сделать нужно как-то хитро.

полноценное оспроизведение будет сделато в отдельной либе player.c 
player.h.

---------------------------------------------------------------------
	WAV_Reset();
	
	while (1) {
		smps = WAV_SendFrame(&wavfile, size);
		
		if (smps == WAV_OK) continue;
		
		if (smps == WAV_COMPLETE) {
			printf("play file complete\n");
			break;
		}
		
		if (smps == WAV_FILE_ERROR) {
			SD_Get_Error(0, WAV_ErrorCode);
			break;
		}
		
	}
---------------------------------------------------------------------
FIL wavfile;
FILINFO wavinf;
char wavname[256];
t_wave_header waveheader;
unsigned short wav_data[2][WAVE_BUF_LEN];

int Read_Wave(void) 
{
	unsigned int res;
	unsigned int n_bts;
	char s_str[5];
	
	char n_buf;
	int n_frame;
	
	int size;
	int burst_size;
	char wave_end = 0;
	
	int min = 0;
	int sec = 0;
	int smps = 0;
	
	s_str[4] = 0;
	
	if (FS_Mounted == 0) {
		printf("SD-Card not detected\n");
		return -1;
	}
	
	sprintf(wavname, "music.wav");
	
	res = f_open(&wavfile, wavname, FA_READ);
	
	if (FR_OK != res){
		printf("Can't open file:%s, [0x%04X]\r\n", wavname, res);
		SD_Get_Error(0, res);
		return -2;
	}
	
	f_stat(wavname, &wavinf);
	
	printf("File open\n"
		"size: %d bytes\n"
		,(int)bmpinf.fsize);
	
	f_read(&wavfile, (unsigned char *)(&waveheader), 44, &n_bts);
	
	printf("Bytes readed: %d\n", n_bts);
	printf("\nWave info:\n");

	s_str[0] = (char)(waveheader.ChunkID & 0x000000ff);
	s_str[1] = (char)((waveheader.ChunkID & 0x0000ff00) >> 8);
	s_str[2] = (char)((waveheader.ChunkID & 0x00ff0000) >> 16);
	s_str[3] = (char)((waveheader.ChunkID & 0xff000000) >> 24);
	
	printf("Chunk ID: %s;\n", s_str);

	s_str[0] = (char)(waveheader.RIFFType & 0x000000ff);
	s_str[1] = (char)((waveheader.RIFFType & 0x0000ff00) >> 8);
	s_str[2] = (char)((waveheader.RIFFType & 0x00ff0000) >> 16);
	s_str[3] = (char)((waveheader.RIFFType & 0xff000000) >> 24);
	
	printf("RIFF Type: %s ", s_str);
	
	s_str[0] = (char)(waveheader.subchunk1Id & 0x000000ff);
	s_str[1] = (char)((waveheader.subchunk1Id & 0x0000ff00) >> 8);
	s_str[2] = (char)((waveheader.subchunk1Id & 0x00ff0000) >> 16);
	s_str[3] = (char)((waveheader.subchunk1Id & 0xff000000) >> 24);
	printf("%s;\n", s_str);
	
	printf("Size: %d \n", waveheader.chunkSize);
	
	printf("Compression: 0x%08X \n", waveheader.audioFormat);
	printf("Channels: %d \n", waveheader.numChannels);
	printf("Sample Rate: %d \n", waveheader.sampleRate);
	printf("Byte Rate: %d \n", waveheader.byteRate);
	printf("Bits per sample: %d \n  \n  \n", waveheader.bitsPerSample);

	draw_time(0, 228, min, sec);
	
	CS43L22_on();
	
	size = (waveheader.chunkSize + 8) - 44;
	burst_size = WAVE_BUF_LEN;
	
	f_lseek(&wavfile, 44);
	f_read(&wavfile, (unsigned char *)(&wav_data[0][0]), WAVE_BUF_LEN * 2, &n_bts);
	I2S_Send_Audio_Buffer((unsigned short *)(&wav_data[0][0]), WAVE_BUF_LEN);
	
	n_buf = 1;
	n_frame = 1;
	
	while(1) {
		
		GPIO_Debounce(GPIOA, 0, &btn1_state);
		if (btn1_state.state == 1 && btn1_state.event == 1) {
			btn1_state.event = 0;
			wave_end = 2;
		}
		
		res = f_lseek(&wavfile, (n_frame * WAVE_BUF_LEN * 2) + 44);
		
		if (res != FR_OK) {
			SD_Get_Error(0, res);
			break;
		}
		
		if (((n_frame + 1) * WAVE_BUF_LEN * 2) > size) {
			burst_size = (((n_frame + 1) * WAVE_BUF_LEN * 2) - size) * 2;
			wave_end = 1;
		}
		
		res = f_read(&wavfile, (unsigned char *)(&wav_data[n_buf][0]), burst_size * 2, &n_bts);
		
		while(I2S_Transfer_Complete == 0);
		I2S_Transfer_Complete = 0;
		while (!(SPI3->SR & SPI_SR_TXE));
		TOGGLE_MARK
		I2S_Send_Audio_Buffer((unsigned short *)(&wav_data[n_buf][0]), burst_size);
		
		if (wave_end > 0) {
			while(I2S_Transfer_Complete == 0);
			I2S_Transfer_Complete = 0;
			while (!(SPI3->SR & SPI_SR_TXE));
			if (wave_end == 1)
				printf("play file complete\n");
			else printf("play file stopped\n");
			
			printf("play time - %d:%d\n", min, sec);
			break;
		}
		
		if (n_buf == 1) n_buf = 0; else n_buf = 1;
		n_frame++;
		
		smps++;
		if (smps > 199) {
			smps = 0;
			sec++;
			if (sec > 59) {
				sec = 0;
				min++;
				if (min > 59) {
					min = 0;
				}
			}
			draw_time(0, 228, min, sec);
		}
	}

	f_close(&wavfile);
	CS43L22_off();
	return 0;
}
*/
