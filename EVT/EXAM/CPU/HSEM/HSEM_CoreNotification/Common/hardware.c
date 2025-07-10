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
#ifdef Core_V3F
vu8 Notif_Rx = 0;
#endif

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

#ifdef Core_V3F

void HSEM_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      HSEM_Handler
 *
 * @brief   This function handles HSEM exception.
 *
 * @return  none
 */
void HSEM_Handler(void)
{
	Notif_Rx =1;
	HSEM_ClearITPendingBit(HSEM_ID0);
}
#endif


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	printf("HSEM_CoreNotification TEST\r\n");
	GPIO_Toggle_INIT();

#ifdef Core_V3F
	vu8 i = 0;
    NVIC_SetPriority(HSEM_IRQn, 0);
	NVIC_EnableIRQ(HSEM_IRQn);

	while(1)
	{
		Notif_Rx = 0;
		HSEM_ITConfig(HSEM_ID0, ENABLE);

		while(Notif_Rx == 0){

		}

		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
		printf("...V3F running\r\n");
	}

#else
	while(1){
		HSEM_FastTake(HSEM_ID0);
		for(u8 t=0; t<5; t++){
			GPIO_SetBits(GPIOA, GPIO_Pin_4);
			Delay_Ms(200);
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);
			Delay_Ms(200);
		}
		printf("..V5F running\r\n");
		HSEM_ReleaseOneSem(HSEM_ID0, 0);
	}

#endif

}
