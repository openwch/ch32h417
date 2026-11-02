/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/10/18
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 SPI uses DMA, master/slave mode transceiver routine:
 Master:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1)\SPI2_NSS(PB12).
 Slave:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1)\SPI2_NSS(PB12).

 This example demonstrates that Master and Slave use DMA full-duplex transmission
 and reception at the same time, repeated every 3s.
 The slave reception is interrupt-driven (DMA1_Channel2 TC interrupt): the ISR
 prints the received data, compares it with the transmitted data (same/diff),
 and restarts the SPI DMA for the next transfer.
 Note: The two boards download the Master and Slave programs respectively, and power
 on at the same time.
     Hardware connection:
               PB13 -- PB13
               PC2 -- PC2
               PC1 -- PC1
               PB12 -- PB12

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
	SystemInit();
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	Delay_Ms(1000);

	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
	Delay_Ms(500);

#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
	NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
	printf("V3F wake up\r\n");

	Hardware();

#elif (Run_Core == Run_Core_V3F)
	Hardware();

#elif (Run_Core == Run_Core_V5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	printf("V3F wake up\r\n");
#endif

	
	while(1)
	{
		printf("V3F running...\r\n");
		Delay_Ms(1000);
	}
}
