/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/16
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
/*********************************************************************
 * @fn      CMP_Init
 *
 * @brief   Initializes CMP.
 *
 * @return  none
 */
void CMP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    CMP_InitTypeDef  CMP_InitTypeDef = {0};

    RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA|RCC_HB2Periph_OPCM, ENABLE );
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF13);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    CMP_InitTypeDef.Mode = OUT_TO_IO;
    CMP_InitTypeDef.NSEL = CMP_CHN0;
    CMP_InitTypeDef.PSEL = CMP_CHP_1;
    CMP_InitTypeDef.VREF = CMP_VREF_OFF;
    OPA_CMP_Init(&CMP_InitTypeDef);
    OPA_CMP_Cmd(ENABLE);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Comparator routines.
 *
 * @return  none
 */
void Hardware(void)
{

	CMP_Init();
	while(1);
}
