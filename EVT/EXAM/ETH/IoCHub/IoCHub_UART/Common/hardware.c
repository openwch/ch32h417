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
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "app_iochub.h"
#include "app_net.h"
#include "app_uart.h"
#include "app_config.h"

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE );

    TIM_TimeBaseStructure.TIM_Period = HCLKClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 100 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_SetPriority (TIM2_IRQn, 0x80);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      AT_Init
 *
 * @brief   Initializes AT CMD port.
 *
 * @return  none
 */

void AT_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_USART1 | RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF7);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART1, &USART_InitStructure);
    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);

    NVIC_EnableIRQ(USART1_IRQn);
    USART_Cmd (USART1, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   IOCHUB Demo
 *
 * @return  none
 */
void Hardware(void)
{
    printf ("IoCHub - Uart\r\n");
    printf ("SystemClk:%lu\r\n", (unsigned long)SystemCoreClock);
    TIM2_Init();
    NET_Init();
    IoCHub_Init();
    AT_Init();
    uart_Init();

    while (1) {
        NET_Process();
        WCHIOCHUB_Process();
        AT_Process();
        if (sessionopenflag & (1 << 0)) {
            IoCHubToComTransData(0,0);
            ComToIoCHubTransData(0,0);
        }
    }
}

