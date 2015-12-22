/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "gpio.h"

/*!
 --------------------------------------------------------------------
 \brief Инициализация портов ввода-вывода
 
 Инициализируем все порты, а для начала нам понадобится
 
 |-------------------------------------------------------------------
 | Pin   | Port | Funcrion     | Type   | Note
 |-------------------------------------------------------------------
 | GPIOA
 |-------------------------------------------------------------------
 | P1_16 | PA4  | Audio_WS     | AF6    | I2S3_WS
 | P1_15 | PA5  | Touch_SCK    | AF5    | SPI1_SCK
 | P1_18 | PA6  | Touch_MISO   | AF5    | SPI1_MISO
 | P1_17 | PA7  | Touch_MOSI   | AF5    | SPI1_MOSI
 |-------------------------------------------------------------------
 | GPIOB
 |-------------------------------------------------------------------
 | P2_23 | PB6  | Audio_SCL    | AF4    | I2C1_SCL
 | P2_20 | PB9  | Audio_SDA    | AF4    | I2C1_SDA
 | P1_35 | PB11 | Memory_SD_CS | Output | PU_PD 
 | P1_36 | PB12 | Memory_EE_CS | Output | PU_PD 
 | P1_37 | PB13 | Memory_SCK   | AF5    | SPI2_SCK
 | P1_38 | PB14 | Memory_MISO  | AF5    | SPI2_MISO
 | P1_39 | PB15 | memory_MOSI  | AF5    | SPI2_MOSI
 |-------------------------------------------------------------------
 | GPIOC
 |-------------------------------------------------------------------
 | P2_48 | PC7  | Audio_MCK    | AF6    | I2S3_MCK
 | P2_44 | PC9  | Marker       | Output | PU_PD
 | P2_37 | PC10 | Audio_SCK    | AF6    | I2S3_CK
 | P2_35 | PC12 | Audio_SD     | AF6    | I2S3_SD
 |-------------------------------------------------------------------
 | GPIOD
 |-------------------------------------------------------------------
 | P2_36 | PD0  | LCD_D2       | AF12   | FSMC_D2
 | P2_33 | PD1  | LCD_D3       | AF12   | FSMC_D3
 | P2_34 | PD2  | T_IRQ        | Input  | Pull_UP 
 | P2_31 | PD3  | T_CS         | Output | PU_PD	    
 | P2_32 | PD4  | LCD_RD       | AF12   | FSMC_NOE
 | P2_29 | PD5  | LCD_WR       | AF12   | FSMC_NWE
 | P2_27 | PD7  | LCD_CS       | AF12   | FSMC_NE1
 | P1_40 | PD8  | LCD_D13      | AF12   | FSMC_D13
 | P1_41 | PD9  | LCD_D14      | AF12   | FSMC_D14
 | P1_42 | PD10 | LCD_D15      | AF12   | FSMC_D15
 | P1_43 | PD11 | LCD_RS       | AF12   | FSMC_A16
 | P1_44 | PD12 | LCD_RST      | Output | PU_PD
 | P1_45 | PD13 | Audio_RST    | Output | PU_PD
 | P1_46 | PD14 | LCD_D0       | AF12   | FSMC_D0
 | P1_47 | PD15 | LCD_D1       | AF12   | FSMC_D1
 |-------------------------------------------------------------------
 | GPIOE
 |-------------------------------------------------------------------
 | P1_25 | PE7  | LCD_D4       | AF12   | FSMC_D4
 | P1_26 | PE8  | LCD_D5       | AF12   | FSMC_D5
 | P1_27 | PE9  | LCD_D6       | AF12   | FSMC_D6
 | P1_28 | PE10 | LCD_D7       | AF12   | FSMC_D7
 | P1_29 | PE11 | LCD_D8       | AF12   | FSMC_D8
 | P1_30 | PE12 | LCD_D9       | AF12   | FSMC_D9
 | P1_31 | PE13 | LCD_D10      | AF12   | FSMC_D10
 | P1_32 | PE14 | LCD_D11      | AF12   | FSMC_D11
 | P1_33 | PE15 | LCD_D12      | AF12   | FSMC_D12
 |-------------------------------------------------------------------
 
 --------------------------------------------------------------------
 */
void GPIO_Init(void)
{
	// GPIOA --------------------------------------------------------
	// Mode, Default 0xA8000000
	GPIOA->MODER |= 0x0001AA00;
	// Out Speed
	GPIOA->OSPEEDR |= 0x0003FF00;
	// Alternate Function
	GPIOA->AFR[0] |= 0x55560000;

	// GPIOB --------------------------------------------------------
	// Mode, Default 0x00000280
	GPIOB->MODER |= 0xA9482000;
	
	// PuPd
	GPIOB->PUPDR |= 
		GPIO_PUPDR_PUPDR14_0 | 
		GPIO_PUPDR_PUPDR6_0 |
		GPIO_PUPDR_PUPDR9_0 |
		GPIO_PUPDR_PUPDR0_0 | 
		GPIO_PUPDR_PUPDR1_0;
	
	GPIOB->OTYPER |= 
		GPIO_OTYPER_ODR_6 |
		GPIO_OTYPER_ODR_9;
	
	// Out Speed
	GPIOB->OSPEEDR |= 0xFFCC3000;
	// Alternate Function
	GPIOB->AFR[0] |= 0x04000000;
	GPIOB->AFR[1] |= 0x55500040;
	
	// GPIOC --------------------------------------------------------
	// Mode
	GPIOC->MODER |= 0x56248000;
	// Out Speed
	GPIOC->OSPEEDR |= 0xFF3CC000;	

	GPIOC->AFR[0] |= 0x60000000;
	GPIOC->AFR[1] |= 0x00060600;
	
	// GPIOD --------------------------------------------------------
	// Mode
	GPIOD->MODER |= 0xA5AA8A4A;
	// Out Speed
	GPIOD->OSPEEDR |= 0xFFFFCFCF;
	// Alternate Function
	GPIOD->AFR[0] |= 0xC0CC00CC;
	GPIOD->AFR[1] |= 0xCC00CCCC;	
	
	// GPIOE --------------------------------------------------------
		// Mode
	GPIOE->MODER |= 0xAAAA8000;
	// Out Speed
	GPIOE->OSPEEDR |= 0xFFFFC000;
	// Alternate Function
	GPIOE->AFR[0] |= 0xC0000000;
	GPIOE->AFR[1] |= 0xCCCCCCCC;
	
	// Initial OUT --------------------------------------------------
	GPIOD->BSRRL |= 
		GPIO_BSRR_BS_12;
		
	GPIOB->BSRRL = 
		GPIO_BSRR_BS_11 |
		GPIO_BSRR_BS_12;
		
	GPIOB->BSRRH = 
		GPIO_BSRR_BS_13;
}

/*!
 --------------------------------------------------------------------
 \brief	Обработка дребезка на входе GPIO
 
 \param	gpio - указатель на порт
 \param	pin - номер лапки порта
 \param	gp_state - указатель на структуру статуса
 --------------------------------------------------------------------
 */
void GPIO_Debounce(GPIO_TypeDef * gpio, 
	unsigned short pin, t_gpio_in_state * gp_state)
{
	unsigned short mask = 0x0001 << pin;
	int dbcnt = 512;
	if (gp_state->state == 1) {
		if (gpio->IDR & mask) {
			while(gp_state->state == 1) {
				if (gpio->IDR & mask) dbcnt++;
				else  dbcnt--;
				if (dbcnt > 1024) {
					gp_state->state = 0;
					gp_state->event = 1;
				} else if (dbcnt < 1) break;
			}
		}
	} else {
		if ((gpio->IDR & mask) == 0) {
			while(gp_state->state == 0) {
				if ((gpio->IDR & mask) == 0)
					dbcnt++;
				else  dbcnt--;
				if (dbcnt > 1024) {
					gp_state->state = 1;
					gp_state->event = 1;
				} else if (dbcnt < 1) break;
			}
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void GPIO_Button_Handler(t_gpio_in_state * btn, int *hldc)
{
	if ((btn->event == 1) && (btn->state == 0)) {
		btn->event = 0; btn->pressed = 1; *hldc = 0;
	}
	
	if (btn->state == 1) {
		if (*hldc < 50) (*hldc)++;
		else { *hldc = 49; btn->hold = 1; }
	}
}
