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
   This example is used to demonstrate the FMC DMA interface operating NOR_SRAM-IS62WV25616BLL.
  PIN:
    FSMC_NADV   -    PB7
    FSMC_NE1   -     PD13 (CS)
    FSMC_NOE    -    PD4  (OE)
    FSMC_NWE    -    PD5  (WE)
    FSMC_NBL1   -    PE1  (UB)
    FSMC_NBL0   -    PE0  (LB)
    FSMC_D0     -    PD14
    FSMC_D1     -    PD15
    FSMC_D2     -    PD0
    FSMC_D3     -    PD1
    FSMC_D4     -    PE7
    FSMC_D5     -    PE8
    FSMC_D6     -    PE9
    FSMC_D7     -    PE10
    FSMC_D8     -    PE11
    FSMC_D9     -    PE12
    FSMC_D10    -    PE13
    FSMC_D11    -    PE14
    FSMC_D12    -    PE15
    FSMC_D13    -    PD8
    FSMC_D14    -    PD9
    FSMC_D15    -    PD10
    FSMC_A16    -    PD11
    FSMC_A17    -    PD12
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
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); 

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

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
