/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2026/10/15
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 * RTC timed wake up stop mode routine:
 * stop mode takes effect only when both V3F core and V5F core enter this mode. 
 * This mode will stop all clocks. After waking up, the system will continue to run, 
 * with HSI being the default system clock.
 * This example demonstrates periodic wake-up from Stop mode using the RTC timer. 
 * After setting the alarm time, the system enters Stop mode. Approximately 5 seconds later, 
 * the RTC automatically wakes up the system, which resumes execution, and the cycle continues. 
 * The internal LSI oscillator is used as the RTC clock source.
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
	printf("V5F SystemCoreClk:%u\r\n", SystemCoreClock);
	Delay_Ms(500);

#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);
	printf("V5F sleep..\r\n");
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
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
