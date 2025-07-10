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
 * This process demonstrates the software configuration method for adjusting the power of the VIO18.
 * To ensure the safety of external devices, it is recommended to use the hardware configuration VIO18.
 *
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
	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
	Delay_Ms(500);

	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
	/* The VIO18 power supply rail is adjusted to 3.3V through software. 
	To ensure the safety of external devices, it is recommended to use hardware configuration. */
	PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
	PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
    Hardware();

#elif (Run_Core == Run_Core_V3F)

    Hardware();

#elif (Run_Core == Run_Core_V5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
#endif

	
    while(1)
    {
		printf("V3F running...\r\n");
		Delay_Ms(1000);
    }
}
