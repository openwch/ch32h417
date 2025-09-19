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
/*********************************************************************
 * @fn      OPA3_Init
 *
 * @brief   Initializes OPA3.
 *
 * @return  none
 */
void OPA3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    OPA_InitTypeDef  OPA_InitStructure = {0};

    RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC|RCC_HB2Periph_OPCM, ENABLE );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    OPA_InitStructure.PSEL = CHP0;
    OPA_InitStructure.NSEL = CHN_PGA_8xIN;
    OPA_InitStructure.Mode=OUT_IO_OUT0;
	OPA_InitStructure.PGADIF=DIF_OFF;
	OPA_InitStructure.FB=FB_ON;
	OPA_InitStructure.HS=HS_ON;
    OPA_Init( OPA3,&OPA_InitStructure );
    OPA_Cmd(OPA3, ENABLE );
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
    OPA3_Init();
	while (1);
}
