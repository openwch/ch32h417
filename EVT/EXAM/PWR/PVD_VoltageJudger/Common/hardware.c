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

volatile uint8_t Voltage_ThresFlag=0;

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
	PWR_PVDLevelConfig(PWR_PVDLevel_MODE7);
	PWR_PVDCmd(ENABLE);

    while (1)
    {
        Delay_Ms(250);
        Voltage_ThresFlag = !PWR_GetFlagStatus(PWR_FLAG_PVDO);
        Voltage_ThresFlag ? printf("**Voltage is higher than rising edge threshold\r\n") : printf("##Voltage is lower than falling edge threshold\r\n");
    }
}
