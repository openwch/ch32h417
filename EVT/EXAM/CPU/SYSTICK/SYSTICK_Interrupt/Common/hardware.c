/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2026/10/15
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global define */

/* Global Variable */
uint32_t counter = 0;

#ifdef Core_V3F
void SysTick0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#else
void SysTick1_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#endif

/*********************************************************************
 * @fn      SYSTICK_Init_Config
 *
 * @brief   SYSTICK_Init_Config.
 *
 * @return  none
 */
void SYSTICK_Init_Config(uint64_t ticks)
{
#ifdef Core_V3F
    SysTick0->ISR &= ~(1 << 0);//clear State flag
    SysTick0->CMP = ticks;
    SysTick0->CNT = 0;
    SysTick0->CTLR = 0xF;

    NVIC_SetPriority(SysTick0_IRQn, 0);
    NVIC_EnableIRQ(SysTick0_IRQn);
#else
    SysTick0->ISR &= ~(1 << 1);//clear State flag
    SysTick1->CMP = ticks;
    SysTick1->CNT = 0;
    SysTick1->CTLR = 0xF;

    NVIC_SetPriority(SysTick1_IRQn, 0);
    NVIC_EnableIRQ(SysTick1_IRQn);
#endif
}


/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Systick interruptTest\r\n");
    SYSTICK_Init_Config(HCLKClock-1);

    while(1)
    {

    }

}

#ifdef Core_V3F
/*********************************************************************
 * @fn      SysTick0_Handler
 *
 * @brief   This function handles SysTick0 exception.
 *
 * @return  none
 */
void SysTick0_Handler(void)
{
    if(SysTick0->ISR == 1)
    {
        SysTick0->ISR &= ~(1<<0);//clear State flag
        counter++;
        #if 0
        printf("welcome to WCH\r\n");
        printf("Counter:%d\r\n",counter);
        #endif
    }
}

#else
/*********************************************************************
 * @fn      SysTick1_Handler
 *
 * @brief   This function handles SysTick1 exception.
 *
 * @return  none
 */
void SysTick1_Handler(void)
{
    if(SysTick0->ISR == (1<<1))
    {
        SysTick0->ISR &= ~(1<<1);//clear State flag
        counter++;
        #if 0
        printf("welcome to WCH\r\n");
        printf("Counter:%d\r\n",counter);
        #endif
    }
}

#endif