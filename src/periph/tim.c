/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "tim.h"

unsigned char delay_flag;


/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void TIM7_config(void) 
{
	NVIC->ISER[1] |= 0x00800000;
	TIM7->PSC |= 83;
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->CR1 |= 
		TIM_CR1_ARPE | 
		TIM_CR1_OPM;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void TIM7_Delay(unsigned short us_delay)
{
	if (us_delay == 0) return;
	TIM7->ARR = us_delay;
	TIM7->CR1 |= TIM_CR1_CEN;
	while (delay_flag == 0);
	delay_flag = 0;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void TIM7_IRQHandler(void)
{
	if (TIM7->SR &= TIM_SR_UIF) {
		TIM7->SR &= ~TIM_SR_UIF;
		delay_flag = 1;
	}
}
