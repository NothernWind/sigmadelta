/*!
 ********************************************************************
 * @file
 * @author
 * @version
 * @date
 * @brief
 *
 ********************************************************************
 */ 
#include "system_irq.h"

// Cortex-M4 Processor Exceptions Handlers --------------------------
/*!
 ********************************************************************
 * @brief	This function handles NMI exception.
 *
 ********************************************************************
 */
void NMI_Handler(void)
{
}

/*!
 ********************************************************************
 * @brief  This function handles Hard Fault exception.
 *
 ********************************************************************
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1) {}
}

/*!
 ********************************************************************
 * @brief  This function handles Memory Manage exception.
 *
 ********************************************************************
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1) {}
}

/*!
 ********************************************************************
 * @brief  This function handles Bus Fault exception.
 *
 ********************************************************************
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1) {}
}

/*!
 ********************************************************************
 * @brief  This function handles Usage Fault exception.
 *
 ********************************************************************
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1) {}
}

/*!
 ********************************************************************
 * @brief  This function handles SVCall exception.
 *
 ********************************************************************
 */
void SVC_Handler(void)
{
}

/*!
 ********************************************************************
 * @brief  This function handles Debug Monitor exception.
 *
 ********************************************************************
 */
void DebugMon_Handler(void)
{
}

/*!
 ********************************************************************
 * @brief  This function handles PendSVC exception.
 *
 ********************************************************************
 */
void PendSV_Handler(void)
{
}

/*!
 ********************************************************************
 * @brief  This function handles SysTick Handler.
 *
 ********************************************************************

void SysTick_Handler(void)
{
} */
