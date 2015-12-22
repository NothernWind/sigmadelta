/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "systick.h"

// Variables --------------------------------------------------------
int enter_btn_hold = 0;
int up_btn_hold = 0;
int down_btn_hold = 0;
int exit_btn_hold = 0;

t_gpio_in_state enter_button  = {0, 0, 0, 0};
t_gpio_in_state up_button = {0, 0, 0, 0};
t_gpio_in_state down_button = {0, 0, 0, 0};
t_gpio_in_state exit_button = {0, 0, 0, 0};

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void SysTick_Init(void)
{
	SysTick->LOAD = 1679999;
	SysTick->CTRL |= 0x06;
	SysTick->CTRL |= 0x01;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void SysTick_Handler(void)
{
	if (SysTick->CTRL & 0x10000) {
		// 1. Обработка нажатия кнопок
		GPIO_Debounce(GPIOA, 0, &enter_button);
		GPIO_Button_Handler(&enter_button, &enter_btn_hold);
		
		GPIO_Debounce(GPIOA, 1, &up_button);
		GPIO_Button_Handler(&up_button, &up_btn_hold);
		
		GPIO_Debounce(GPIOA, 2, &down_button);
		GPIO_Button_Handler(&down_button, &down_btn_hold);
		
		GPIO_Debounce(GPIOA, 3, &exit_button);
		GPIO_Button_Handler(&exit_button, &exit_btn_hold);
	}
}

