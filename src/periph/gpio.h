/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef GPIO_H
#define GPIO_H

#include "main.h"

#define SDCARD_CS_ON	GPIOB->BSRRH |= GPIO_BSRR_BS_11;
#define SDCARD_CS_OFF	GPIOB->BSRRL |= GPIO_BSRR_BS_11;
#define EEPROM_CS_ON	GPIOB->BSRRH |= GPIO_BSRR_BS_12;
#define EEPROM_CS_OFF	GPIOB->BSRRL |= GPIO_BSRR_BS_12;
#define LCD_RST_ON		GPIOD->BSRRH |= GPIO_BSRR_BS_12;
#define LCD_RST_OFF		GPIOD->BSRRL |= GPIO_BSRR_BS_12;
#define AUDIO_RST_ON	GPIOD->BSRRH |= GPIO_BSRR_BS_13;
#define AUDIO_RST_OFF	GPIOD->BSRRL |= GPIO_BSRR_BS_13;

#define MARK_ON		GPIOC->BSRRL |= GPIO_BSRR_BS_9;
#define MARK_OFF	GPIOC->BSRRH |= GPIO_BSRR_BS_9;
#define TOGGLE_MARK\
	if (GPIOC->ODR &= GPIO_ODR_ODR_9)\
		GPIOC->BSRRH |= GPIO_BSRR_BS_9;\
	else GPIOC->BSRRL |= GPIO_BSRR_BS_9;

// Board Leds
#define LED1_ON		GPIOA->BSRRL |= GPIO_BSRR_BS_8;
#define LED1_OFF	GPIOA->BSRRH |= GPIO_BSRR_BS_8;
#define LED1_TOGGLE\
	if (GPIOA->ODR &= GPIO_ODR_ODR_8)\
		GPIOA->BSRRH |= GPIO_BSRR_BS_8;\
	else GPIOA->BSRRL |= GPIO_BSRR_BS_8;

#define LED2_ON		GPIOC->BSRRL |= GPIO_BSRR_BS_13;
#define LED2_OFF	GPIOC->BSRRH |= GPIO_BSRR_BS_13;
#define LED2_TOGGLE\
	if (GPIOC->ODR &= GPIO_ODR_ODR_13)\
		GPIOC->BSRRH |= GPIO_BSRR_BS_13;\
	else GPIOC->BSRRL |= GPIO_BSRR_BS_13;

#define LED3_ON		GPIOC->BSRRL |= GPIO_BSRR_BS_14;
#define LED3_OFF	GPIOC->BSRRH |= GPIO_BSRR_BS_14;
#define LED3_TOGGLE\
	if (GPIOC->ODR &= GPIO_ODR_ODR_14)\
		GPIOC->BSRRH |= GPIO_BSRR_BS_14;\
	else GPIOC->BSRRL |= GPIO_BSRR_BS_14;

#define LED4_ON		GPIOC->BSRRL |= GPIO_BSRR_BS_15;
#define LED4_OFF	GPIOC->BSRRH |= GPIO_BSRR_BS_15;
#define LED4_TOGGLE\
	if (GPIOC->ODR &= GPIO_ODR_ODR_15)\
		GPIOC->BSRRH |= GPIO_BSRR_BS_15;\
	else GPIOC->BSRRL |= GPIO_BSRR_BS_15;
	
typedef struct _t_gpio_in_state {
	unsigned state:1;
	unsigned event:1;
	unsigned pressed:1;
	unsigned hold:1;
	unsigned :4;
} t_gpio_in_state;

extern t_gpio_in_state enter_button;
extern t_gpio_in_state up_button;
extern t_gpio_in_state down_button;
extern t_gpio_in_state exit_button;

void GPIO_Init(void);
void GPIO_Button_Debounce(void);
void GPIO_Debounce(GPIO_TypeDef * gpio, unsigned short pin, t_gpio_in_state * gp_state);
void GPIO_Button_Handler(t_gpio_in_state * btn, int *hldc);

#endif // GPIO_H
