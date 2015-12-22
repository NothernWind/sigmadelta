/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "graph.h"

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void draw_pixel(int x, int y, unsigned int color)
{
	S6D1121_set_polygon(x, y, x + 1, y + 1);
	mFSMC_WriteData(color);
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void draw_line(int start_x, int start_y, 
	int end_x, int end_y , unsigned short color)
{
	int dx = abs(end_x - start_x);
	int dy = abs(end_y - start_y);
	int sx = (start_x < end_x) ? 1 : -1;
	int sy = (start_y < end_y) ? 1 : -1;
	int error = dx - dy;
	int error2;
	
	for(;;) {
		draw_pixel(start_x, start_y, color);
		if (start_x == end_x && start_y == end_y) break;
		
		error2 = error * 2;
		
		if (error2 > -dy) error -= dy, start_x += sx;
		if (error2 < dx) error += dx, start_y += sy;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void draw_rect(int x, int y, int w, int h, unsigned short color)
{
	int i = 0;
	S6D1121_set_polygon(x, y, x + w - 1, y + h - 1);
	for (i = 0; i < w * h; i++) {
		mFSMC_WriteData(color);
	}
}
