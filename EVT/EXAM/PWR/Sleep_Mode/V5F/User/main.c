/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *low power, sleep mode routine:
 * This routine demonstrates WFI\WFE enters sleep mode, v3f and v5f need to be configured with different interrupt numbers
 * or event lines.Input a high level at pin PB1 triggers thet EXTI_Line1 to wake up V3F core, and input a high level at 
 * pin PA11 triggers the EXTI_Line11 to wake up V5F core.
 */

#include "debug.h"
#include "hardware.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);
	Delay_Ms(500);
#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);
	Hardware();

#elif (Run_Core == Run_Core_V3F)
	
#elif (Run_Core == Run_Core_V5F)
	Hardware();

#endif

	while(1)
	{
		printf("V5F running...\r\n");
		Delay_Ms(1000);
	}
}
