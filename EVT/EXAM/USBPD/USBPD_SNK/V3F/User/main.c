/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.3
 * Date               : 2026/10/14
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *
 * PD SNK Sample code
 *
 * This sample code may have compatibility issues and is for learning purposes only.
 
 * Make sure that the board is not powered on before use.
 * Be sure to pay attention to the voltage when changing the request
 * to prevent burning the board.
 *
 * There is no integrated 5.1K pull-down inside the chip,
 * CC_PD is only for status differentiation,
 * bit write 1 means SNK mode, write 0 means SRC mode
 *
 * Modify "PDO_Request( PDO_INDEX_1 )" in pd process.c,to modify the request voltage.
 *
 * According to the usage scenario of PD SNK, whether
 * it is removed or not should be determined by detecting
 * the Vbus voltage, this code only shows the detection
 * and the subsequent communication flow.
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
	Delay_Ms(100);
	USART_Printf_Init(115200);
	Delay_Ms(1000);

	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
  


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

	}
}

