/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2026/09/24
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
ETH_IAP example, TCP IAP upgrade with BIM/USER/BACKUP flash layout.
*/
#include "debug.h"
#include "hardware.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemInit();
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	Delay_Ms(1000);

	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
	Delay_Ms(500);

	/* ETH_IAP APP bring-up: V3F only. Do not wake V5 - its SW_Handler
	 * also pending-jumps to 0x30000 and would crash on V3 APP image. */
	Hardware();

    while(1)
    {

    }
}
