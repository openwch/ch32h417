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
 * bit write 1 means SNK mode, write 0 means SCR mode
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
	SystemAndCoreClockUpdate();
	Delay_Init();

	USART_Printf_Init(115200);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);


#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);
	Hardware();

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
	Hardware();
#endif


	while(1)
	{

	}
}

