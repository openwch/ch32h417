/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
   FSMC routine to operate NANDFLASH:
      This routine demonstrates the operation of W29N01HV model NANFLASH erase-read-write-read through the FSMC interface, and in the process of reading and writing
  ECC check.
  PIN:
    FSMC_NOE--PD4  (RE)
    FSMC_NWE--PD5  (WE)
    FSMC_NE1_NCE2--PD7  (CE)
    FSMC_NWAIT--PD6  (R/B)
    FSMC_D0 --PD14
    FSMC_D1 -- PD15
    FSMC_D2 -- PD0
    FSMC_D3 -- PD1
    FSMC_D4 -- PE7
    FSMC_D5 -- PE8
    FSMC_D6 -- PE9
    FSMC_D7 -- PE10
    FSMC_A16 -- PD11 (CLE)
    FSMC_A17 -- PD12 (ALE)
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


#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
	NVIC->SCTLR |= 1<<4;
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

	}
}
