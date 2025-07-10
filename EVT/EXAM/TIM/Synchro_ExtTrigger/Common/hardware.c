/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Slave Mode Definition */
#define RESET_MODE      0
#define GATED_MODE      1
#define TRIGGER__MODE   2

/*  Slave Mode Selection */
//#define SLAVE_MODE   RESET_MODE
//#define SLAVE_MODE   GATED_MODE
#define SLAVE_MODE   TRIGGER__MODE

/*********************************************************************
 * @fn      TIM1_Synchro_ExtTrigger_Init
 *
 * @brief   Initializes TIM1 external trigger synchronization.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void TIM1_Synchro_ExtTrigger_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_ICInitTypeDef TIM_ICInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOE|RCC_HB2Periph_AFIO|RCC_HB2Periph_TIM1, ENABLE );
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOE, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit( TIM1, &TIM_ICInitStructure );

	TIM_SelectInputTrigger( TIM1, TIM_TS_TI2FP2 );

#if (SLAVE_MODE == RESET_MODE)
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Reset );

#elif (SLAVE_MODE == GATED_MODE)
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Gated );

#elif (SLAVE_MODE == TRIGGER__MODE)
	TIM_SelectMasterSlaveMode( TIM1, TIM_SlaveMode_Trigger );

#endif

	TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable );

#if (SLAVE_MODE != TRIGGER__MODE)
	TIM_Cmd( TIM1, ENABLE );

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
	printf("TIM1_Synchro_ExtTrigger\r\n");
	TIM1_Synchro_ExtTrigger_Init( 0xFFFF, 14400-1);
	while(1)
	{
		printf("cnt:%d\r\n", TIM1->CNT);

	}
	while (1)
		;
}
