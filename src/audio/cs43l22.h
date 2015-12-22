/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef CS43L22_H
#define CS43L22_H

#include "main.h"

#define REG_ID					0x01
#define REG_POWER_CTL1			0x02
#define REG_POWER_CTL2			0x04
#define REG_CLOCKING_CTL		0x05
#define REG_INTERFACE_CTL1		0x06
#define REG_INTERFACE_CTL2		0x07
#define REG_PASSTHR_A_SELECT	0x08
#define REG_PASSTHR_B_SELECT	0x09
#define REG_ANALOG_ZC_SR_SETT	0x0A
#define REG_PASSTHR_GANG_CTL	0x0C
#define REG_PLAYBACK_CTL1		0x0D
#define REG_MISC_CTL			0x0E
#define REG_PLAYBACK_CTL2		0x0F
#define REG_PASSTHR_A_VOL		0x14
#define REG_PASSTHR_B_VOL		0x15
#define REG_PCMA_VOL			0x1a
#define REG_PCMB_VOL			0x1b
#define REG_BEEP_FREQ_ON_TIME	0x1c
#define REG_BEEP_VOL_OFF_TIME	0x1d
#define REG_BEEP_TONE_CFG		0x1e
#define REG_TONE_CTL			0x1f
#define REG_MASTER_A_VOL		0x20
#define REG_MASTER_B_VOL		0x21
#define REG_HEADPHONE_A_VOL		0x22
#define REG_HEADPHONE_B_VOL		0x23
#define REG_SPEAKER_A_VOL		0x24
#define REG_SPEAKER_B_VOL		0x25
#define REG_CH_MIXER_SWAP		0x26
#define REG_LIMIT_CTL1			0x27
#define REG_LIMIT_CTL2			0x28
#define REG_LIMIT_ATTACK_RATE	0x29
#define REG_OVF_CLK_STATUS		0x2e
#define REG_BATT_COMPENSATION	0x2f
#define REG_VP_BATTERY_LEVEL	0x30
#define REG_SPEAKER_STATUS		0x31
#define REG_TEMPMONITOR_CTL		0x32
#define REG_THERMAL_FOLDBACK	0x33
#define REG_CHARGE_PUMP_FREQ	0x34


#define CS43L22_I2C_ERROR		-1
#define CS43L22_I2S_ERROR		-1

int CS43L22_Init(int smprate);
void CS43L22_DeInit(void);
int CS43L22_on(void);
int CS43L22_off(void);
int CS43L22_VolumeControl(void);

#endif // CS43L22_H
