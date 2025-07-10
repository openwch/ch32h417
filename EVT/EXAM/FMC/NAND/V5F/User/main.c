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
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);


#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
	Hardware();
#endif


	while(1)
	{
	}
}
