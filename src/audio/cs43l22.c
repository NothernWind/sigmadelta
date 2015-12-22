/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "cs43l22.h"

// Variables --------------------------------------------------------
unsigned char CS43L22_Volume = 0xC0;
int vol_changed = 0;
float vol = 0;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int CS43L22_Init(int smprate)
{
	//CS43L22_Volume = 0xC0;

	AUDIO_RST_ON
	TIM7_Delay(10000);
	AUDIO_RST_OFF
	
	I2C_Init();
	
	I2C1_Device_Addr = 0x94;
	
	// Настройка кодека
	// PDN = 0x01, Powered Down	
	I2C_IRQ_Write(REG_POWER_CTL1, 0x01);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// PDN_HP = 2, Headphone channel is always ON.
	// PDN_HP = 3, Speaker channel is always OFF.
	I2C_IRQ_Write(REG_POWER_CTL2, 0xAF);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// AUTO = 1, Auto-detection of Speed Mode enabled
	// MCLKDIV2 = 1, MCLK signal into DAC divided by 2
	I2C_IRQ_Write(REG_CLOCKING_CTL, 0x81);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// DACDIF = 1, DAC Interface Format: I2S, up to 24-bit data
	I2C_IRQ_Write(REG_INTERFACE_CTL1, 0x0B);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;	
	
	// Максимальная громкость: +12 dB
	I2C_IRQ_Write(REG_MASTER_A_VOL, CS43L22_Volume/*0x18*/);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	I2C_IRQ_Write(REG_MASTER_B_VOL, CS43L22_Volume/*0x18*/);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// Определение нуля не надо, да и чего-то там ещё
	I2C_IRQ_Write(REG_ANALOG_ZC_SR_SETT, 0x00);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;

	// DIGSFT = 0, Does not occur with a soft ramp
	// DEEMPHASIS = 1, config. digital de-emphasis filter response
	I2C_IRQ_Write(REG_MISC_CTL, 0x04);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;

	// Отключить ограничитель
	I2C_IRQ_Write(REG_LIMIT_CTL1, 0x00);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// Подстроить уровни
	// TREB = 0b0000, treble gain +12.0 dB
	// BASS = 0b1111, gain of the bass shelving filter -10.5 dB
	I2C_IRQ_Write(REG_TONE_CTL, 0x0f);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	// Подстроить громколсть обоих каналов
	// PCMxVOL: +5 dB	
	I2C_IRQ_Write(REG_PCMA_VOL, 0x0a);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;	
	
	I2C_IRQ_Write(REG_PCMB_VOL, 0x0a);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	
	if (I2S_Init(smprate) != 0) return -2;
	
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void CS43L22_DeInit(void)
{
	AUDIO_RST_ON
	I2S_DeInit();
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int CS43L22_on(void)
{
	I2C_IRQ_Write(REG_POWER_CTL1, 0x9E);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int CS43L22_off(void)
{
	I2C_IRQ_Write(REG_PLAYBACK_CTL2, 0xFF);
	while (I2C_Status == 1);
	if (I2C_Status != 0) return -1;
	return 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
int CS43L22_VolumeControl(void)
{
	char str[40];
	
	if (vol_changed == 0) {
		// Переместить курсор вверх по списку
		if (up_button.pressed == 1 || up_button.hold == 1) {
			up_button.pressed = 0; 
			up_button.hold = 0;
			
			if (CS43L22_Volume < 0xFF) {
				CS43L22_Volume++;
				vol_changed = 1;
			}
		}
		
		if (down_button.pressed == 1 || down_button.hold == 1) {
			down_button.pressed = 0; 
			down_button.hold = 0;
			
			if (CS43L22_Volume > 0x34) {
				CS43L22_Volume--;
				vol_changed = 1;
			}
		}
		vol = (float)(-1) * ((float)(256 - (unsigned int)(CS43L22_Volume)) / (float)2);
	}
	
	switch (vol_changed) {
		case 1:
			I2C_IRQ_Write(REG_MASTER_A_VOL, CS43L22_Volume);
			vol_changed++;
			break;
		case 2:
			if (I2C_Status == 1) break;
		
			if (I2C_Status == 0) {
				I2C_IRQ_Write(REG_MASTER_B_VOL, CS43L22_Volume);
				vol_changed++;
				break;
			}
			
			if (I2C_Status < 0) {
				vol_changed = -1;
			}
			break;
		case 3:
			if (I2C_Status == 1) break;
			if (I2C_Status == 0) {
				vol_changed = 0;
				diplay_fgcolor.c = 0xFFFF;
				display_bgcolor.c = 0;
				sprintf(str, "Volume: %3.1f db", vol);
				display_print_text(0, 18, str);
			}
			if (I2C_Status < 0) {
				vol_changed = -1;
			}
			break;
		default : break;
	}
	return vol_changed;
}
