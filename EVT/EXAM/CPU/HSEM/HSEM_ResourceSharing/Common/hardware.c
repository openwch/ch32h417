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
	//PA0
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
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

#ifdef Core_V3F
    vu8 i=0;
	GPIO_Toggle_INIT();
	while(1)
	{
		if(HSEM_FastTake(HSEM_ID0) == READY){
			for(u8 t=0; t<10; t++){
				GPIO_WriteBit(GPIOA, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
				printf("...V3F running\r\n");
				Delay_Ms(300);
			}
			HSEM_ReleaseOneSem(HSEM_ID0, 0);
			Delay_Ms(10);
		}
	}

#else
    vu8 i=0;

	while(1)
	{
		if(HSEM_FastTake(HSEM_ID0) == READY){
			for(u8 t=0; t<5; t++){
				GPIO_WriteBit(GPIOA, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
				printf("...V5F running\r\n");
				Delay_Ms(800);
			}
			HSEM_ReleaseOneSem(HSEM_ID0, 0);
			Delay_Ms(10);
		}
	}

#endif

}
