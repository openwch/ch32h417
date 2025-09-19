/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/07/17
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
   This example is used to demonstrate the FMC interface operating SDRAM-IS42S32800G by DMA.
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
   DQM2  -----PC10
   DQM3  -----PA15
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
   D16   -----PC3
   D17   -----PB5
   D18   -----PF6
   D19   -----PF7
   D20   -----PF8
   D21   -----PF9
   D22   -----PF10
   D23   -----PA0
   D24   -----PA1
   D25   -----PA2
   D26   -----PA3
   D27   -----PA4
   D28   -----PA5
   D29   -----PB0
   D30   -----PB1
   D31   -----PB2
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
  printf("SDRAM demo\n");
#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
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
