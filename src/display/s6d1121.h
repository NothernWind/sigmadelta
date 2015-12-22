/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef S6D1121_H
#define S6D1121_H

#include "main.h"

// Defines ----------------------------------------------------------

#define PRODICTION_CODE			0x00
#define DRIVER_OUTPUT_CTRL		0x01
#define DRIVING_WAVEFORM_CTRL	0x02
#define ENTRY_MODE				0x03
#define OSCILLATOR_CTRL			0x04
#define DISPLAY_CTRL			0x07
#define BLANK_PERIOD_CTRL1		0x08
#define FRAME_CYCLE_CTRL		0x0A //!< +0x00 +0x01
#define EXTERBAL_INTERFACE_CTRL	0x0C
#define POWER_CTRL				0x10 //!< 1-7 0x10-0x16
#define GRAM_A7_A0				0x20
#define GRAM_A16_A8				0x21
#define GRAM_D15_D00			0x22
#define GAMMA_CTRL				0x30 //!< 1-14 0x30-0x3D
#define VERTICAL_SCROLL_CTRL	0x41
#define SCREEN1_END_POS			0x42
#define SCREEN1_START_POS		0x43
#define SCREEN2_END_POS			0x44
#define SCREEN2_START_POS		0x45
#define H_WINDOW_ADDR			0x46
#define V_WINDOW_ADDR_END		0x47
#define V_WINDOW_ADDR_START		0x48
#define MDDI_WU_CTRL			0x50
#define MDDI_LINK_WU_SP			0x51
#define SUB_PANEL_CTRL			0x52 //!< 1-3 0x52-0x54
#define GPIO_VALUE				0x55
#define GPIO_IO_CTRL			0x56
#define GPIO_CLEAR				0x57
#define GPIO_IE					0x58
#define GPIO_PS					0x59
#define MTP_INIT				0x60
#define MTP_VCOMH_READ			0x61
#define SET_MTP_TEST_KEY		0x62
#define GOE_TIMING				0x70
#define GATE_START_PUSLE		0x71
#define RED_OST					0x72
#define GREEN_OST				0x73
#define BLUE_OST				0x74
#define RSW_TIMING				0x75
#define GSW_TIMING				0x76
#define BSW_TIMING				0x77
#define VCOM_OUTPUT_CTRL		0x78
#define PANEL_SIGNAL_CTRL		0x79 //!< 1-2 9x79-0x7A

// Typedefs ---------------------------------------------------------
typedef struct _t_cfg_header {
	unsigned short header;
	unsigned short id;
	int itemsize;//bytes
	int items;
} t_cfg_header;

typedef struct _t_cfg_item {
	unsigned short rADDR;
	unsigned short rVALUE;
	int delay;
} t_cfg_item;

// Prototypes -------------------------------------------------------
void S6D1121_init(void);
void S6D1121_clear_screen(unsigned short color);
void S6D1121_set_polygon(int start_x, int start_y, int end_x, int end_y);

#endif // S6D1121_H
