/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/24
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
  FMC routine to operate LCD drived by ST7789:
  LCD--PIN:
    PD7 --CS
    PD11--RS
    PD5 --FMC_NEW(WR)
    PD4 --FMC_NOE(RD)
    PA15--LCDRST#
    PD14--FMC_D0
    PD15--FMC_D1
    PD0 --FMC_D2
    PD1--FMC_D3
    PE7--FMC_D4
    PE8 --FMC_D5
    PE9 --FMC_D6
    PE10--FMC_D7
    PA12--BL
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
		printf("V5F running...\r\n");
		Delay_Ms(1000);
	}
}
