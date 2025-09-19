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
   This example is used to demonstrate the FMC DMA interface operating NorFlash-MT28EW128ABA1LJS-0SIT.
  PIN:
   (Latches)
    FMC_NOE      -    PD4  (OE)
    FMC_NWE      -    PD5  (WE)
    FMC_NWAIT    -    PD6  (RY/BY#)
    FMC_NE1      -    PD7  (CE)
    FMC_A0       -    PF5
    FMC_A1       -    PB3
    FMC_A2       -    PB4
    FMC_A3       -    PB8
    FMC_A4       -    PB9
    FMC_A5       -    PC13
    FMC_A6       -    PA10
    FMC_A7       -    PA11
    FMC_A8       -    PA12
    FMC_A9       -    PA13
    FMC_A10      -    PB10
    FMC_A11      -    PB11
    FMC_A12      -    PB12
    FMC_A13      -    PB13
    FMC_A14      -    PB14
    FMC_A15      -    PB15
    FMC_A16      -    PD11
    FMC_A17      -    PD12
    FMC_A18      -    PD13
    FMC_A19      -    PE3
    FMC_A20      -    PE4
    FMC_A21      -    PE5
    FMC_A22      -    PE6
	FMC_D0       -    PD14 
    FMC_D1       -    PD15
    FMC_D2       -    PD0
    FMC_D3       -    PD1
    FMC_D4       -    PE7
    FMC_D5       -    PE8
    FMC_D6       -    PE9
    FMC_D7       -    PE10
    FMC_D8       -    PE11
    FMC_D9       -    PE12
    FMC_D10      -    PE13
    FMC_D11      -    PE14
    FMC_D12      -    PE15
    FMC_D13      -    PD8
    FMC_D14      -    PD9
    FMC_D15      -    PD10
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
