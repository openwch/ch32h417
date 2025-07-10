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
 SPI uses DMA, master/slave mode transceiver routine:
 *Master:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1).
 *Slave:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1).
 
 This example demonstrates that Master and Slave use DAM full-duplex transmission
 and reception at the same time.
 Note: The two boards download the Master and Slave programs respectively, and power
 on at the same time.
     Hardware connection:
               PB13 -- PB13
               PC2 -- PC2
               PC1 -- PC1
 
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
