/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/07/17
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
   This example is used to demonstrate the FMC interface operating SDRAM-W9825G6KH by DMA.
   Pin:
   SDCLK -----PF2
   SDCKE0-----PC5
   SDNE0 -----PC2
   BA0   -----PB14
   BA1   -----PB15
   NRAS  -----PF11
   NCAS  -----PF12
   SDNWE -----PC0
   DQM0  -----PC12
   DQM1  -----PC11
   A0    -----PF5
   A1    -----PB3
   A2    -----PB4
   A3    -----PB8
   A4    -----PB9
   A5    -----PB6
   A6    -----PA10
   A7    -----PA11
   A8    -----PA12
   A9    -----PA13
   A10   -----PB10
   A11   -----PB11
   A12   -----PB12
   D0    -----PD14
   D1    -----PD15
   D2    -----PD0
   D3    -----PD1
   D4    -----PE7
   D5    -----PE8
   D6    -----PE9
   D7    -----PE10
   D8    -----PE11
   D9    -----PE12
   D10   -----PE13
   D11   -----PE14
   D12   -----PE15
   D13   -----PD8
   D14   -----PD9
   D15   -----PD10
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
