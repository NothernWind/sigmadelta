/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "main.h"

// Определим цвета
#define C_RED			0xF800
#define C_DARK_RED		0x8000
#define C_GREEN			0x07E0
#define C_DARK_GREEN	0x0400
#define C_BLUE			0x001F
#define C_DARK_BLUE		0x0010
#define C_CYAN			0x07FF
#define C_DARK_CYAN		0x0410
#define C_MAGENTA		0xF81F
#define C_DARK_MAGENTA	0x8010
#define C_YELLOW		0xFFE0
#define C_DARK_YELLOW	0x8400
#define C_BLACK 		0x0000
#define C_GRAY			0xC618
#define C_DARK_GRAY		0x8410
#define C_WHITE 		0xFFFF

#define RGB565CONVERT(red, green, blue) \
	(int) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

// Prototypes -------------------------------------------------------
typedef union _t_color {
	unsigned short c;
	struct {
		unsigned blue:5;
		unsigned green:6;
		unsigned red:5;
	} rgb;
} t_color;

// Extern variables -------------------------------------------------
extern t_color diplay_fgcolor;
extern t_color display_bgcolor;

extern const char error_icon[32][32];
extern const char warning_icon[32][32];
extern const char info_icon[32][32];
extern const char pixfont8x12[256][12];
extern const char pixfont5x8[128][5];

// Prototypes -------------------------------------------------------
void display_init(void);
void display_write_data(int start_x, int start_y, int end_x, 
	int end_y, const unsigned short *buf, int size);
void display_draw_char(int x, int y, unsigned short color, char ch);
void display_print_char(int x, int y, char ch);
void display_draw_text(int x, int y, unsigned short color, const char * text);
void display_clear(void);
void display_fputc(int c);
void display_print_text(int x, int y, const char * text);
void draw_time(int x, int y, int min, int sec);

void display_draw_char_v2(int x, int y, unsigned short color, char ch);
void display_print_icon_32x32(int x, int y, const char icon[32][32]);

unsigned short get_icon_pixel(char c);

void display_draw_eq(int x, int y, unsigned short * buf);

#endif // DISPLAY_H
