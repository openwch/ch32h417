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
 *DVP--PIN:
 *   D0--PE0(AF11)
 *   D1--PE1(AF11)
 *   D2--PE2(AF13)
 *   D3--PE3(AF13)
 *   D4--PE4(AF13)
 *   D5--PB6(AF13)
 *   D6--PE5(AF13)
 *   D7--PE6(AF13)
 *   D8--PC10(AF13)
 *   D9--PC12(AF13)
 *   D10--PD6(AF13)
 *   D11--PD2(AF13)
 *   VSYNC--PB7(AF13)
 *   HERF--PB13(AF8)
 *   DPCLK--PB12(AF15)
 *   SDA--PC1
 *   SDCLK--PC0
 *   DPWDN--PB0
 *   RESET--PB1
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
	USART_Printf_Init(921600);
	Delay_Ms(1000);
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
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
	printf("V3F wake up\r\n");
	Hardware();

#elif (Run_Core == Run_Core_V3F)
	Hardware();

#elif (Run_Core == Run_Core_V5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
#endif	
	while(1)
	{
		printf("V3F running...\r\n");
		Delay_Ms(1000);
	}
}
