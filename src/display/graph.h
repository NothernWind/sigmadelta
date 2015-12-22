/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef GRAPH_H
#define GRAPH_H

#include "main.h"

void draw_pixel(int x, int y, unsigned int color);

void draw_line(int start_x, int start_y, 
	int end_x, int end_y , unsigned short color);

void draw_rect(int x, int y, int w, int h, unsigned short color);

#endif // GRAPH_H
