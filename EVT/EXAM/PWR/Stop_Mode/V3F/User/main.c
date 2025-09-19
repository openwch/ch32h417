/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/07/29
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 * low power, stop mode routine:
 * stop mode takes effect only when both V3F core and V5F core enter this mode. 
 * This mode will stop all clocks. After waking up, the system will continue to run, 
 * with HSI being the default system clock.
 * This routine demonstrates WFI\WFE enters stop mode, v3f and v5f need to be configured with different interrupt numbers
 * or event lines.Input a high level at pin PB1 triggers the EXTI_Line1 to wake up V3F core, and input a high level 
 * at pin PA11 triggers the EXTI_Line11 to wake up V5F core.
 *@Note
 * For the small package model of the chip, there are some pins that have not been led out compared to the largest package, 
 * or some pins that have been packaged but not used. These pins need to be set as pull-down\up inputs to reduce power 
 * consumption.Please refer to the routine configuration for details. 
 */

#include "debug.h"
#include "hardware.h"

/*********************************************************************
 * @fn      GPIO_ALL_IPD
 *
 * @brief   To reduce power consumption, unused I/O is configured in pull-down input mode.
 *
 * @return  none
 */
void GPIO_ALL_IPD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |
						  RCC_HB2Periph_GPIOD| RCC_HB2Periph_GPIOE | RCC_HB2Periph_GPIOF, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

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
	Delay_Ms(1000);
	GPIO_ALL_IPD();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
	Delay_Ms(500);
	/*For chips with externally sealed VIO18 pins, if users use 3.3V and do not require low voltage support, 
	VDDIO and VIO18 can be short circuited off chip. By configuring VSEL_VIO18 in the software to turn off LDO,
	power consumption can be reduced. For chips that do not include the VIO18 pin, VDDIO and VIO18 are short 
	circuited on the chip. After power on, LDO needs to be turned off in the software through VSEL_VIO18 to 
	reduce power consumption.*/
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
	PWR_VIO18LevelCfg(PWR_VIO18Level_MODE5);

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
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

#endif

    while(1)
    {
		printf("V3F running...\r\n");
		Delay_Ms(1000);
    }
}
