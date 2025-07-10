/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417_it.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
  while (1)
  {
    // USART_Printf_Init(115200);
    // printf("42222222222222\r\n");
  }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  // NVIC_SystemReset();
  while (1)
  {
    USART_Printf_Init(115200);

    printf("__get_MTVAL():%08x\r\n",__get_MTVAL());
    printf("__get_MEPC():%08x\r\n",__get_MEPC());
    printf("__get_MCAUSE():%08x\r\n",__get_MCAUSE());
    printf("0x20100570:%08x\r\n",*(vu32*)0x20100570);
    printf("442222\r\n");
  }
}





