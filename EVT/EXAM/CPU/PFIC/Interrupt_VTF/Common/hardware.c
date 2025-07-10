/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global define */

/* Global Variable */
uint32_t time=0;



#ifdef Core_V3F
void SysTick0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#else
void SysTick1_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#endif

/*********************************************************************
 * @fn      Interrupt_VTF_Init
 *
 * @brief   Initializes VTF.
 *
 * @return  none
 */
void Interrupt_VTF_Init(void)
{
#ifdef Core_V3F
    SetVTFIRQ((u32)SysTick0_Handler,SysTick0_IRQn,0,ENABLE);
    NVIC_EnableIRQ(SysTick0_IRQn);
#else
    SetVTFIRQ((u32)SysTick1_Handler,SysTick1_IRQn,0,ENABLE);
    NVIC_EnableIRQ(SysTick1_IRQn);
#endif
}


/*********************************************************************
 * @fn      Systick_Init
 *
 * @brief   Initializes Systick.
 *
 * @return  none
 */
void Systick_Init(void)
{
#ifdef Core_V3F
    SysTick0->ISR &= ~(1 << 0);
    SysTick0->CNT = 0;
    SysTick0->CMP = 0x20;
    SysTick0->CTLR = 0x7;
#else
    SysTick0->ISR &= ~(1 << 1);
    SysTick1->CNT = 0;
    SysTick1->CMP = 0x20;
    SysTick1->CTLR = 0x7;
#endif
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Interrupt VTF Test\r\n");

    Interrupt_VTF_Init();
    Systick_Init();

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
    time=SysTick0->CNT;
    SysTick0->CTLR=0;
    SysTick0->ISR &= ~(1<<0);
    printf("delta time:%d\r\n",time-0x20);
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
    time=SysTick1->CNT;
    SysTick1->CTLR=0;
    SysTick0->ISR &= ~(1<<1);
    printf("delta time:%d\r\n",time-0x20);
}

#endif