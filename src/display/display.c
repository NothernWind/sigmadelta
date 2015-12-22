/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "display.h"

// Prototypes -------------------------------------------------------

// Variables --------------------------------------------------------
t_color diplay_fgcolor;
t_color display_bgcolor;

char display_text_buffer[20][40];
int current_chr_pos;
int current_str_pos;
int display_tb_offset;
char display_tb_ovf;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_init(void)
{
	S6D1121_init();
	display_clear();
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_write_data(int start_x, int start_y, int end_x, 
	int end_y, const unsigned short *buf, int size)
{
	S6D1121_set_polygon(start_x, start_y, end_x, end_y);
	FSMC_DMA_Write(buf, size);
}

/*!
 --------------------------------------------------------------------
 \brief Нарисовать символ
 
 --------------------------------------------------------------------
 */
void display_draw_char(int x, int y, unsigned short color, char ch)
{
	int i;
	int j;
	unsigned char mask;
	S6D1121_set_polygon(x, y, x+7, y+11);
	
	for (i = 0; i < 12; i++) {
		mask = 0x80;
		for (j = 0; j < 8; j++) {
			if ((unsigned char)pixfont8x12[ch][i] & mask) {
				mFSMC_WriteData(color);
			} else {
				mFSMC_WriteData(0);
			}
			mask >>= 1;
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief Нарисовать символ
 
 --------------------------------------------------------------------
 */
void display_print_char(int x, int y, char ch)
{
	int i;
	int j;
	unsigned char mask;
	S6D1121_set_polygon(x * 8, y * 12, (x * 8) + 7, (y * 12) + 11);
	
	for (i = 0; i < 12; i++) {
		mask = 0x80;
		for (j = 0; j < 8; j++) {
			if ((unsigned char)pixfont8x12[ch][i] & mask) {
				mFSMC_WriteData(diplay_fgcolor.c);
			} else {
				mFSMC_WriteData(display_bgcolor.c);
			}
			mask >>= 1;
		}
	}	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_print_text(int x, int y, const char * text)
{
	int i = 0;
	int temp = x;
	for (i = 0; text[i]; i++) {
		if (text[i] != '\n') {
			display_print_char(temp++, y, text[i]);
		} else {
			y++;
			temp = x;
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief нарисовать значок размером 32 на 32 пикселя
 
 --------------------------------------------------------------------
 */
void display_print_icon_32x32(int x, int y, const char icon[32][32])
{
	int i;
	int j;
	S6D1121_set_polygon(x, y, x + 31, y + 31);
	
	for (i = 0; i < 32; i++) {

		for (j = 0; j < 32; j++) {
			if (icon[i][j] == '.') {
				mFSMC_WriteData(display_bgcolor.c);
			} else {
				mFSMC_WriteData(get_icon_pixel(icon[i][j]));
			}
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_draw_eq(int x, int y, unsigned short * buf)
{
	int i;
	int j;
	unsigned short s;
	for (j = 0; j < 64; j++) {
		S6D1121_set_polygon(x + j, y, j + x, y + 31);
		s = 32 - (buf[j] / 2048);
		for (i = 0; i < 32; i++) {
			if (s > i) {
				mFSMC_WriteData(display_bgcolor.c);
			} else {
				mFSMC_WriteData(diplay_fgcolor.c);
			}
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void draw_time(int x, int y, int min, int sec)
{
	int i = 0;
	char c;
	
	for (i = 0; i < 5; i++) {
		switch(i) {
			case 0: 
				c = ((min / 10) % 10) + 0x30;
				break;
			case 1: 
				c = (min % 10) + 0x30;
				break;
			case 2: 
				c = ':';
				break;
			case 3: 
				c = ((sec / 10) % 10) + 0x30;
				break;
			case 4: 
				c = (sec % 10) + 0x30;
				break;
			default:
				break;
		}

		display_draw_char(x + (i*8), y, 0xFFFF, c);
	}
}

/*!
 --------------------------------------------------------------------
 \brief Нарисовать текст на экране. Текст начинается рисоваться посимвольно
 
 \param x - Начало текстового полигона по оси Х
 \param y - Начало текстового полигона по оси Y
 \param text - указатель на текстовый буфер
 
 --------------------------------------------------------------------
 */
void display_draw_text(int x, int y, unsigned short color, const char * text)
{
	int i = 0;
	for (i = 0; text[i]; i++) {
		display_draw_char(x + i * 8, y, color, text[i]);
	}
}

// Реализация функций для retarget и printf -------------------------


/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_clear(void) 
{
	int i, j;
	for (i = 0; i < 20; i++) {
		for (j = 0; j < 40; j++) {
			display_text_buffer[i][j] = 0;
		}
	}
	current_chr_pos = 0;
	current_str_pos = 0;
	display_tb_offset = 0;
	display_tb_ovf = 0;
	S6D1121_clear_screen(0);
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_draw_string(int n_str, const char * str)
{
	int i;
	char c;
	
	for (i = 0; i < 40; i++) {
		if (str[i] == '\0') c = 1;
		display_draw_char(i * 8, n_str * 12, 0xFFFF, ((c == 0) ? str[i]: ' '));
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void display_text_up(int offset) 
{
	int nstr;
	for (nstr = 0; nstr < 19; nstr++) {
		if (offset >= 20) offset = 0;
		display_draw_string(nstr, (char *)(&display_text_buffer[offset++][0]));
		
	}
	display_draw_string(19, "\0");
}

/*!
 --------------------------------------------------------------------
 \brief
 
Сделаем так, что бы каждый вход в эту функцию просто добавлял символ
в конец строки. и выводил его сразу на эран. т.е. вошло в функцию
добавило символ в текстовый буфер и плюхнуло этот же символ сразу же на
экран.

если строка переполнилась или встретили символ переноса строки, то
просто продолжаем выводить символы сразу с начала следующей строки.

 --------------------------------------------------------------------
 */
void display_fputc(int c) 
{
	static char f = 0;
	
	if (c == '\r') return;
	
	if (c == '\n') {
		display_text_buffer[current_str_pos][current_chr_pos] = '\0';
		current_chr_pos = 0;
		current_str_pos++;
		f = 1;
		if (current_str_pos >= 20) {
			current_str_pos = 0;
			if (display_tb_ovf == 0)
				display_tb_ovf = 1;
		}
	} else {
		if (display_tb_ovf == 0) {
			display_draw_char(current_chr_pos * 8, current_str_pos * 12, 0xFFFF, c);
			display_text_buffer[current_str_pos][current_chr_pos++] = c;
			
			if (current_chr_pos >= 40) {
				current_chr_pos = 0;
				current_str_pos++;
				if (current_str_pos >= 20) {
					current_str_pos = 0;
					display_tb_ovf = 1;
				}
			}
		} else {
			if (f == 1) {
				f = 0;
				display_text_up(current_str_pos + 1);
			}
			
			display_draw_char(current_chr_pos * 8, 19 * 12, 0xFFFF, c);
			display_text_buffer[current_str_pos][current_chr_pos++] = c;
			
			if (current_chr_pos >= 40) {
				current_chr_pos = 0;
				f = 1;
				current_str_pos++;
				if (current_str_pos >= 20) {
					current_str_pos = 0;
					
				}
			}	
		}
	}
}
