/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef WAV_H
#define WAV_H

#include "main.h"

#define WAV_COMPLETE	2
#define WAV_I2S_BUSY	1
#define WAV_OK			0
#define WAV_FILE_ERROR	-1

/*!
 ********************************************************************
 * \brief	Структура WAV-файла
 *
 * \param	chunkId - Содержит символы “RIFF” в ASCII кодировке 
 *			(0x52494646 в big-endian представлении). Является началом 
 *			RIFF-цепочки.
 * \param	chunkSize - Это оставшийся размер цепочки, начиная с этой 
 *			позиции. Иначе говоря, это размер файла – 8, то есть, исключены 
 *			поля chunkId и chunkSize.
 * \param	format - Содержит символы “WAVE” (0x57415645 в big-endian 
 *			представлении)
 * \param	subchunk1Id - Содержит символы “fmt “ (0x666d7420 в big-endian 
 *			представлении)
 * \param	subchunk1Size - 16 для формата PCM. Это оставшийся размер 
 *			подцепочки, начиная с этой позиции.
 * \param	audioFormat - Аудио формат, полный список можно получить здесь. 
 *			Для PCM = 1 (то есть, Линейное квантование). Значения, отличающиеся 
 *			от 1, обозначают некоторый формат сжатия.
 * \param	numChannels - Количество каналов. Моно = 1, Стерео = 2 и т.д.
 * \param	sampleRate - Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
 * \param	byteRate - Количество байт, переданных за секунду воспроизведения.
 * \param	blockAlign - Количество байт для одного сэмпла, включая все каналы.
 * \param	bitsPerSample - Количество бит в сэмпле. Так называемая “глубина” 
 *			или точность звучания. 8 бит, 16 бит и т.д.
 * \param	subchunk2Id - Содержит символы “data” (0x64617461 в big-endian 
 *			представлении)
 * \param	subchunk2Size - Количество байт в области данных.
 ********************************************************************
 */

typedef struct _t_wave_header {
 	unsigned int ChunkID;
 	unsigned int chunkSize;
 	unsigned int RIFFType;
 	unsigned int subchunk1Id;
 	unsigned int subchunk1Size;
 	unsigned short audioFormat;
 	unsigned short numChannels;
 	unsigned int sampleRate;
 	unsigned int byteRate;
 	unsigned short blockAlign;
 	unsigned short bitsPerSample;
 	unsigned int subchunk2Id;
 	unsigned int subchunk2Size;
} t_wave_header;

extern int WAV_ErrorCode;

void WAV_Play(const char * wav_name);

#endif // WAV_H
