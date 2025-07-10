/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global define */

/* Global Variable */


/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0.
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
#ifdef Core_V3F
	//PA0
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#else
	//PA4
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	printf("HSEM_CoreSync TEST\r\n");
	GPIO_Toggle_INIT();

#ifdef Core_V3F
    vu8 i=0;
	Delay_Ms(50);
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);

	while(1)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
		printf("...V3F running\r\n");
		Delay_Ms(1000);
	}

#else
	HSEM_ITConfig(HSEM_ID0, ENABLE);
	NVIC->SCTLR |= 1<<4;
	while(HSEM_GetOneSemTakenState(HSEM_ID0) == 0){

	}

	while(1){
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		Delay_Ms(500);
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		Delay_Ms(500);
		printf("..V5F running\r\n");
	}

#endif

}
