/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/07/16
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/*********************************************************************
 * @fn      GPIO_MCOInit
 *
 * @brief   Initializes RCC_MCO
 *
 * @return  none
 */
void GPIO_MCOInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB|RCC_HB2Periph_AFIO, ENABLE);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF0);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	RCC_MCOConfig(RCC_MCO_HSI);
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
	RCC_HBPeriphClockCmd(RCC_HBPeriph_CRC, ENABLE);

	RCC_ClocksTypeDef RCC_ClocksStatus={0};
    RCC_GetClocksFreq(&RCC_ClocksStatus);
	printf("SYSCLK_Frequency-%d\r\n", RCC_ClocksStatus.SYSCLK_Frequency);
    printf("HCLK_Frequency-%d\r\n", RCC_ClocksStatus.HCLK_Frequency);
    printf("Core_Frequency-%d\r\n", RCC_ClocksStatus.Core_Frequency);
	printf("ADCCLK_Frequency-%d\r\n", RCC_ClocksStatus.ADCCLK_Frequency);
    GPIO_MCOInit();

	while (1)
		;
}
