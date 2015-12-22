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

/*
R - red
r - dark red
G - green
g - dark green
B - blue
b - dark blue
C - cyan
c dark cyan
M - magenta
m - dark magenta
Y - yellow
y - dark yellow
# - black
S - gray
s - dark gray
W - white
. - no color
*/
const char error_icon[32][32] = {
"................................",
"............rrrrrrrr............",
".........rrrRRRRRRRRrrr.........",
"........rRRRRRRRRRRRRRRr........",
"......rrRRRRRRRRRRRRRRRRrr......",
".....rRRRRRRRRRRRRRRRRRRRRr.....",
"....rRRRRRRRRRRRRRRRRRRRRRRr....",
"....rRRRRRRRRRRRRRRRRRRRRRRr....",
"...rRRRRRRWRRRRRRRRRRWRRRRRRr...",
"..rRRRRRRWWWRRRRRRRRWWWRRRRRRr..",
"..rRRRRRWWWWWRRRRRRWWWWWRRRRRr..",
"..rRRRRRRWWWWWRRRRWWWWWRRRRRRr..",
".rRRRRRRRRWWWWWRRWWWWWRRRRRRRRr.",
".rRRRRRRRRRWWWWWWWWWWRRRRRRRRRr.",
".rRRRRRRRRRRWWWWWWWWRRRRRRRRRRr.",
".rRRRRRRRRRRRWWWWWWRRRRRRRRRRRr.",
".rRRRRRRRRRRRWWWWWWRRRRRRRRRRRr.",
".rRRRRRRRRRRWWWWWWWWRRRRRRRRRRr.",
".rRRRRRRRRRWWWWWWWWWWRRRRRRRRRr.",
".rRRRRRRRRWWWWWRRWWWWWRRRRRRRRr.",
"..rRRRRRRWWWWWRRRRWWWWWRRRRRRr..",
"..rRRRRRWWWWWRRRRRRWWWWWRRRRRr..",
"..rRRRRRRWWWRRRRRRRRWWWRRRRRRr..",
"...rRRRRRRWRRRRRRRRRRWRRRRRRr...",
"....rRRRRRRRRRRRRRRRRRRRRRRr....",
"....rRRRRRRRRRRRRRRRRRRRRRRr....",
".....rRRRRRRRRRRRRRRRRRRRRr.....",
"......rrRRRRRRRRRRRRRRRRrr......",
"........rRRRRRRRRRRRRRRr........",
".........rrrRRRRRRRRrrr.........",
"............rrrrrrrr............",
"................................"};

const char warning_icon[32][32] = {
"................................",
"..............yyy...............",
".............yYYY#..............",
"............yYYYYY#.............",
"............yYYYYY#.............",
"...........yYYYYYYY#............",
"...........yYYYYYYY#............",
"..........yYYYYYYYYY#...........",
"..........yYYYYYYYYY#...........",
".........yYYYYYYYYYYY#..........",
".........yYYYS###SYYY#..........",
"........yYYYY#####YYYY#.........",
"........yYYYY#####YYYY#.........",
".......yYYYYY#####YYYYY#........",
".......yYYYYY#####YYYYY#........",
"......yYYYYYY#####YYYYYY#.......",
"......yYYYYYYy###yYYYYYY#.......",
".....yYYYYYYYS###SYYYYYYY#......",
".....yYYYYYYYY###YYYYYYYY#......",
"....yYYYYYYYYYy#yYYYYYYYYY#.....",
"....yYYYYYYYYYS#SYYYYYYYYY#.....",
"...yYYYYYYYYYYY#YYYYYYYYYYY#....",
"...yYYYYYYYYYYYYYYYYYYYYYYY#....",
"..yYYYYYYYYYYYS##SYYYYYYYYYY#...",
"..yYYYYYYYYYYY####YYYYYYYYYY#...",
".yYYYYYYYYYYYY####YYYYYYYYYYY#..",
".yYYYYYYYYYYYYS##SYYYYYYYYYYY#..",
".yYYYYYYYYYYYYYYYYYYYYYYYYYYY#..",
".yYYYYYYYYYYYYYYYYYYYYYYYYYYY#..",
"..yYYYYYYYYYYYYYYYYYYYYYYYYY#...",
"...y########################....",
"................................"};

const char info_icon[32][32] = {
"................................",
"............bbbbbbbb............",
".........bbbBBBBBBBBbbb.........",
"........bBBBBBBBBBBBBBBb........",
"......bbBBBBBBBBBBBBBBBBbb......",
".....bBBBBBBBBBBBBBBBBBBBBb.....",
"....bBBBBBBBBBBBBBBBBBBBBBBb....",
"....bBBBBBBBBBWWWBBBBBBBBBBb....",
"...bBBBBBBBBBWWWWWBBBBBBBBBBb...",
"..bBBBBBBBBBBWWWWWBBBBBBBBBBBb..",
"..bBBBBBBBBBBWWWWWBBBBBBBBBBBb..",
"..bBBBBBBBBBBBWWWBBBBBBBBBBBBb..",
".bBBBBBBBBBBBBBBBBBBBBBBBBBBBBb.",
".bBBBBBBBBBBBWWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
".bBBBBBBBBBBBBWWWWBBBBBBBBBBBBb.",
"..bBBBBBBBBBBBWWWWBBBBBBBBBBBb..",
"..bBBBBBBBBBBBWWWWBBBBBBBBBBBb..",
"..bBBBBBBBBBBBWWWWBBBBBBBBBBBb..",
"...bBBBBBBBBBBWWWWBBBBBBBBBBb...",
"....bBBBBBBBBWWWWWWBBBBBBBBb....",
"....bBBBBBBBBBBBBBBBBBBBBBBb....",
".....bBBBBBBBBBBBBBBBBBBBBb.....",
"......bbBBBBBBBBBBBBBBBBbb......",
"........bBBBBBBBBBBBBBBb........",
".........bbbBBBBBBBBbbb.........",
"............bbbbbbbb............",
"................................"};

/*
 ********************************************************************
 * 
 * R - red
 * r - dark red
 * G - green
 * g - dark green
 * B - blue
 * b - dark blue
 * C - cyan
 * c dark cyan
 * M - magenta
 * m - dark magenta
 * Y - yellow
 * y - dark yellow
 * # - black
 * S - gray
 * s - dark gray
 * W - white
 * . - no color
 *********************************************************************
 */
unsigned short get_icon_pixel(char c)
{
	unsigned short color;
	
	switch(c) {		
		case 'R': color = C_RED; break;			
		case 'r': color = C_DARK_RED; break;		
		case 'G': color = C_GREEN; break;			
		case 'g': color = C_DARK_GREEN; break;	
		case 'B': color = C_BLUE; break;			
		case 'b': color = C_DARK_BLUE; break;		
		case 'C': color = C_CYAN; break;		
		case 'c': color = C_DARK_CYAN; break;	
		case 'M': color = C_MAGENTA; break;			
		case 'm': color = C_DARK_MAGENTA; break;		
		case 'Y': color = C_YELLOW; break;		
		case 'y': color = C_DARK_YELLOW; break;	
		case '#': color = C_BLACK; break; 		
		case 'S': color = C_GRAY; break;			
		case 's': color = C_DARK_GRAY; break;
		case 'W': color = C_WHITE; break; 
		default: color = 0; break;
	}
	return color;
}
