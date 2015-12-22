/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "s6d1121.h"


t_cfg_item s6d1121_config[43] = {
	{POWER_CTRL + 0x01, 0x2004, 0},
	{POWER_CTRL + 0x03, 0xCC00, 0},
	{POWER_CTRL + 0x05, 0x2600, 0},
	{POWER_CTRL + 0x04, 0x252A, 0},
	{POWER_CTRL + 0x02, 0x0033, 0},
	{POWER_CTRL + 0x03, 0xCC04, 0},
	{POWER_CTRL + 0x03, 0xCC06, 0},
	{POWER_CTRL + 0x03, 0xCC4F, 0},
	{POWER_CTRL + 0x03, 0x674F, 0},
	{POWER_CTRL + 0x01, 0x2003, 0},
	{GAMMA_CTRL, 0x2609, 0},
	{GAMMA_CTRL + 0x01, 0x242C, 0},
	{GAMMA_CTRL + 0x02, 0x1F23, 0},
	{GAMMA_CTRL + 0x03, 0x2425, 0},
	{GAMMA_CTRL + 0x04, 0x2226, 0},
	{GAMMA_CTRL + 0x05, 0x2523, 0},
	{GAMMA_CTRL + 0x06, 0x1C1A, 0},
	{GAMMA_CTRL + 0x07, 0x131D, 0},
	{GAMMA_CTRL + 0x08, 0x0B11, 0},
	{GAMMA_CTRL + 0x09, 0x1210, 0},
	{GAMMA_CTRL + 0x0A, 0x1315, 0},
	{GAMMA_CTRL + 0x0B, 0x3619, 0},
	{GAMMA_CTRL + 0x0C, 0x0D00, 0},
	{GAMMA_CTRL + 0x0D, 0x000D, 0},
	{POWER_CTRL + 0x06, 0x0007, 0},
	{DRIVING_WAVEFORM_CTRL, 0x0013, 0},
	{ENTRY_MODE, 0x0009, 0}, 
	{DRIVER_OUTPUT_CTRL, 0x0127, 0},
	{BLANK_PERIOD_CTRL1, 0x0303, 0},
	{FRAME_CYCLE_CTRL, 0x000B, 0},
	{FRAME_CYCLE_CTRL + 0x01, 0x0003, 0},
	{EXTERBAL_INTERFACE_CTRL, 0x0000, 0},
	{VERTICAL_SCROLL_CTRL, 0x0000, 0},
	{MDDI_WU_CTRL, 0x0000, 0},
	{MTP_INIT, 0x0005, 0},
	{GOE_TIMING, 0x000B, 0},
	{GATE_START_PUSLE, 0x0000, 0},
	{VCOM_OUTPUT_CTRL, 0x0000, 0},
	{PANEL_SIGNAL_CTRL + 0x01, 0x0000, 0},
	{PANEL_SIGNAL_CTRL, 0x0007, 0},
	{DISPLAY_CTRL, 0x0051, 0},
	{DISPLAY_CTRL, 0x0053, 0},
	{PANEL_SIGNAL_CTRL, 0x0000, 0}
};

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void S6D1121_init(void)
{
	int i;
	
	LCD_RST_ON
	TIM7_Delay(10000);
	LCD_RST_OFF
	
	for (i = 0; i < 43; i++) {
		mFSMC_WriteReg(s6d1121_config[i].rADDR, s6d1121_config[i].rVALUE);
		if (s6d1121_config[i].delay > 5) TIM7_Delay(s6d1121_config[i].delay);
	}
}

/*!
 --------------------------------------------------------------------
 \brief очистка дисплея
 
 \param color - цвет которым нуно залить всё поле
 
 --------------------------------------------------------------------
 */
void S6D1121_clear_screen(unsigned short color)
{
	int i = 0;
	mFSMC_WriteReg(H_WINDOW_ADDR, 0xEF00);
	mFSMC_WriteReg(V_WINDOW_ADDR_END, 0x13F);//VEA
	mFSMC_WriteReg(V_WINDOW_ADDR_START, 0x00);
	mFSMC_WriteReg(GRAM_A7_A0, 0);
	mFSMC_WriteReg(GRAM_A16_A8, 0);
	mFSMC_WriteReg(GRAM_D15_D00, 0);
	
	for (i = 0; i < 320 * 240; i++) {
		mFSMC_WriteData(color);	
	}
}

/*!
 --------------------------------------------------------------------
 \brief Установить полигон для вывода.
 
 --------------------------------------------------------------------
 */
void S6D1121_set_polygon(int start_x, int start_y, int end_x, int end_y)
{
	mFSMC_WriteReg(H_WINDOW_ADDR, (end_y << 8) + start_y);
	mFSMC_WriteReg(V_WINDOW_ADDR_END, 319 - start_x);
	mFSMC_WriteReg(V_WINDOW_ADDR_START, 319 - end_x);
	mFSMC_WriteReg(GRAM_A7_A0, start_y);
	mFSMC_WriteReg(GRAM_A16_A8, 319 - start_x);
	mFSMC_WriteIndex(GRAM_D15_D00);
}
