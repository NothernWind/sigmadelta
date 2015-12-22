/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef MSG_H
#define MSG_H

#include "main.h"

#define MSG_INFORMATION		1
#define MSG_WARNING			2
#define MSG_CRITICAL		3

extern int MSG_Error_ID;	// Посути тут нужно чказать где произошла ошибка
extern unsigned int MSG_FF_Error;	// Коды ошибок fatfs

void msg_print_border(int msg_type);
void msg_sd_ejected(void);
void msg_ff_error(int error_code);
void msg_mp3_warning(void);
void msg_adio_codec_error(int error_code);
void msg_mp3_unk_error(int error_code);
void msg_travel(void);

#endif	// MSG_H
