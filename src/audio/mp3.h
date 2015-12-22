/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef MP3_H
#define MP3_H

#include "main.h"

#define MP3_OBUF_SIZE	4096	// Размер выходного буфера
#define MP3_FBUF_SIZE	MP3_OBUF_SIZE * 4 // размера файлового буфера

#define MP3_FS_ERROR		-1
#define MP3_CODEC_ERROR		-2
#define MP3_HARDWARE_ERROR	-3
#define MP3_BAD_SAMPLE_RATE	-4
#define MP3_UNKNOWN_ERROR	-5

void MP3_Play(const char * mp3_name);


#endif
