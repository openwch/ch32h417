/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/10/20
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
  8 lines SDMMC routine to operate eMMC card:
  This example demonstrates reading and writing all sectors of the eMMC card--KLM8G1GETF-B041
  through the SDMMC interface .
  SDMMC--PIN:
    D0--PC8
    D1--PC9
    D2--PC10
    D3--PC11
    D4--PA14
    D5--PA15
    D6--PC6
    D7--PC7
    CMD--PD2
    SCK--PC12
    STR--PD3

     Note: Except for SCK and STR , the rest need to pull up 10K resistors
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
