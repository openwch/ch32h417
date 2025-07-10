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
  }
}

extern uint32_t FinalOprateAddress;

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
      /* The code is printing out information related to a hard fault exception. */
    printf("In the function HardFault_Handler\r\n");
    printf("Last Oprate Address is %#010x\r\n", FinalOprateAddress);
    printf("MCAUSE: %#02x\r\n", __get_MCAUSE());
    printf("MEPC: %#010x\r\n", __get_MEPC());
    /* The code is using a switch statement to check the value of the `__get_MCAUSE()` function.
    Depending on the value returned by this function, different actions are taken. */
    switch (__get_MCAUSE())
    {
        case 1:
            printf("The address you excuse is in pmp range!!\r\n");
            break;
        case 7:
            printf("The address you Store is in pmp range!!\r\n");
            break;
        case 5:
            printf("The address you read is in pmp range!!\r\n");
            break;

        default:
            break;
    }
    while (1)
    {
    }
  while (1)
  {
  }
}


