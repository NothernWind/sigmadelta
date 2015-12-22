/*!
 ********************************************************************
 * @file    system_stm32f4xx.c
 * @author
 * @version
 * @date
 * @brief   
 *             
 * System Clock source PLL (HSE)
 *                                     
 * SYSCLK             168 MHz
 * HCLK               168 MHz
 * AHB                Prescaler 1
 * APB1               Prescaler 4
 * APB2               Prescaler 2
 * HSE                Frequency 8 MHz
 * PLL_M              8
 * PLL_N              336
 * PLL_P              2
 * PLL_Q              7
 * PLLI2S_N           NA
 * PLLI2S_R           NA
 * I2S input clock    NA
 * VDD                3,3 V
 * Main regulator output voltage Scale1 mode
 * Flash Latency(WS)  5
 * Prefetch Buffer    OFF
 * Instruction cache  ON
 * Data cache         ON
 ******************************************************************** 
 */

#include "stm32f4xx.h"

#ifdef DATA_IN_ExtSRAM
#	error "No extram is connected fo this project"
#endif

/*
 * PLL_VCO = (Fosc / PLL_M) * PLL_N  = (8 Мгц / 8) * 336 = 336 Мгц
 * Где Fosc - частота HSE или HSI 
 * SYSCLK = PLL_VCO / PLL_P = 336 Мгц / 2 = 168 МГц
 * USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ  = 336 Мгц / 7 = 48 МГц
 */
#define PLL_M      8
#define PLL_N      336
#define PLL_P      2
#define PLL_Q      7

unsigned int SystemCoreClock;

__I uint8_t AHBPrescTable[16] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

static void SetSysClock(void);

/*!
 ********************************************************************
 * \brief  Настройка микроконтроллерной системы Инициализация интерфейса 
 * встроенной флеш,  PLL и обновление глобальной переменной SystemFrequency.
 *
 ********************************************************************
 */
void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	// Включаем FPU
	SCB->CPACR |= 0x00F00000;
#endif
	RCC->CR |= (uint32_t)0x00000001;//RCC clock control register (RCC_CR)
// HSION включен по умолчанию - непонятное затягивание времени.... может
// там какой косяк в кристале

	// Reset CFGR register
	RCC->CFGR = 0x00000000;
  
	/**************************************************************** 
	 * Reset HSEON, CSSON and PLLON bits 
	 * Reset HSEON - выкл. внешний генератор(кварц), 
	 * CSSON - выкл. система безопасности тактового сигнала and 
	 * PLLON - выкл. главный делитель тактового сигнала bits 
	 ****************************************************************/
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	/**************************************************************** 
	 * Reset PLLCFGR register 
	 * RCC PLL configuration register (RCC_PLLCFGR): Reset value: 0x2400 3010
	 ****************************************************************/
	RCC->PLLCFGR = 0x24003010;

	// Reset HSEBYP bit : HSE clock bypass 
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	// Disable all interrupts 
	RCC->CIR = 0x00000000;

	/****************************************************************
	 * Конфигурирование источника системного тактового сигнала, PLL 
	 * умножителя/делителей, делителей шин AHB/APBx и настроки тайм-аутов 
	 * чтения команд из внутренней флеш
	 ****************************************************************/
	SetSysClock();
	
// Enable HSE

	
	// Configure the Vector Table location add offset address 
#ifdef VECT_TAB_SRAM
	SCB->VTOR = SRAM_BASE;
#else
	SCB->VTOR = FLASH_BASE;
#endif
	SCB->AIRCR = 0x05FA0700;
}

static void SetSysClock(void)
{
	volatile unsigned int StartUpCounter = 0, HSEStatus = 0;
	SystemCoreClock = 16000000;
	
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
   // Ждём по тайм-ауту: запустится ли внешний генератор HSE
	do {
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));	

	if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
		HSEStatus = (uint32_t)0x01;
	} else {
		HSEStatus = (uint32_t)0x00;// иначе - 0x00
	}
	
	if (HSEStatus == (uint32_t)0x01) {
		// Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;
		
		// HCLK = SYSCLK
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
		
		// PCLK2 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
		
		// PCLK1 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
		
		// Configure the main PLL
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
			(RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
		
		// Enable the main PLL
		RCC->CR |= RCC_CR_PLLON;
		
		// Wait till the main PLL is ready
		while((RCC->CR & RCC_CR_PLLRDY) == 0);
		
		// Configure Flash prefetch, Instruction cache, Data cache and wait state
		FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

		// Select the main PLL as system clock source
		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		// Wait till the main PLL is used as system clock source 
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
		SystemCoreClock = 168000000;
	}	
}

/*
#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)
#define AIRCR_VECTKEY_MASK    		((uint32_t)0x05FA0000)

//! 0 bits for pre-emption priority 4 bits for subpriority
#define NVIC_PriorityGroup_0         ((uint32_t)0x700) 
															
void NVIC_Configuration(void)
{
#ifdef VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}

void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  // Check the parameters 
  assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
  assert_param(IS_NVIC_OFFSET(Offset));  
   
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  // Check the parameters 
  assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));
  
  // Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value 
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}

#ifdef VECT_TAB_RAM
SCB->VTOR = 0x20000000;
#else
SCB->VTOR = 0x08000000;
#endif
SCB->AIRCR = 0x05FA0700;

*/

/*!
 * @brief  Конфигурирование источника тактирования системы.
 * 
 *  PLL умножителя/делителей,
 * делителей шин AHB/APBx и настроки тайм-аутов чтения команд из внутренней флеш
 * Эта функция должна быть вызвана только однажды при конфигурация тактового сигнала RCC  
 * после сброса микропроцессора. По умолчанию эта ф-ция уже вызывается из ф-ции SystemInit(). 

static void SetSysClock(void)
{
  volatile unsigned int StartUpCounter = 0, HSEStatus = 0;
  
	// Enable HSE
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
   // Ждём по тайм-ауту: запустится ли внешний генератор HSE
	do {
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
		HSEStatus = (uint32_t)0x01;
	} else {
		HSEStatus = (uint32_t)0x00;// иначе - 0x00
	}
	
	if (HSEStatus == (uint32_t)0x01) {
		// Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;
		
		// HCLK = SYSCLK
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
		
		// PCLK2 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
		
		// PCLK1 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
		
		// Configure the main PLL
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
			RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q << 24);
		
		// Enable the main PLL
		RCC->CR |= RCC_CR_PLLON;
		
		// Wait till the main PLL is ready
		while((RCC->CR & RCC_CR_PLLRDY) == 0);
		
		// Configure Flash prefetch, Instruction cache, Data cache and wait state
		FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

		// Select the main PLL as system clock source
		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_PLL;//Выбран делитель PLL в качестве источника тактового сигнала(т.е. внешний кварц через PLL)

     Wait till the main PLL is used as system clock source 
		ЖДЁМ БЕСКОНЕЧНО - пока PLL не будет выбрана в качестве источника тактового сигнала
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
	}
	else
	{ If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error 
		Если HSE не запустился, то бедет неправильная (не ожидаемая мной) тактовая частота
         конфигурации. Пользователь может добавить здесь некоторый код, чтобы иметь дело с этой ошибкой*
			Короче работаем тут напрямую от 16 МГц внутреннего генератора
	}

}
 */
/*
		!< SW configuration 
SW: Выбор источника тактового сигнала.
00: HSI oscillator selected as system clock
01: HSE oscillator selected as system clock
10: PLL selected as system clock
11: not allowed

#define  RCC_CFGR_SW                         ((uint32_t)0x00000003)        !< SW[1:0] bits (System clock Switch) 
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001)        !< Bit 0 
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002)        !< Bit 1 

#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000000)        !< HSI selected as system clock 
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000001)        !< HSE selected as system clock 
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000002)        !< PLL selected as system clock 

SWS: Показывает - какой источник тактового сигнала выбран
Устанавливается и сбрасывается аппаратно - показывает: какой источник тактового сигнала сейчас используется.
00: HSI oscillator used as the system clock
01: HSE oscillator used as the system clock
10: PLL used as the system clock
11: not applicable

!< SWS configuration 
#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000C)        !< SWS[1:0] bits (System Clock Switch Status) 
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004)        !< Bit 0 
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008)        !< Bit 1 

#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000000)        !< HSI oscillator used as system clock 
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000004)        !< HSE oscillator used as system clock 
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x00000008)        !< PLL used as system clock 
*/


