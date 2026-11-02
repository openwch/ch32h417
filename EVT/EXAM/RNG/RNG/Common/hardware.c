/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/15
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	u32 random = 0;
	printf("CRC TEST\r\n");
	RCC_HBPeriphClockCmd(RCC_HBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    while(1)
    {
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET)
        {
        }
        random=RNG_GetRandomNumber();
        printf("random:0x%08x\r\n",random);
        Delay_Ms(500);
    }
}
