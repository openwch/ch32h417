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
 Single pulse output routine:
 TIM2_CH1(PA0),TIM2_CH2(PB3)
 This routine demonstrates that in single-pulse mode, when a rising edge is detected
 on the TIM2_CH2(PB3) pin, the TIM2_CH1(PA0) outputs positive pulse.
 
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

	}
}
