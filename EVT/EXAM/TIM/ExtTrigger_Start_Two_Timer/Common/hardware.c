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

/*********************************************************************
 * @fn      ExtTrigger_Start_Two_TIM
 *
 * @brief   Starting 2 timers synchronously in response to an external trigger.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void ExtTrigger_Start_Two_TIM( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_ICInitTypeDef TIM_ICInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOE|RCC_HB2Periph_AFIO|RCC_HB2Periph_TIM1, ENABLE );
	RCC_HB1PeriphClockCmd( RCC_HB1Periph_TIM2, ENABLE );

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
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit( TIM1, &TIM_ICInitStructure );

	TIM_SelectInputTrigger( TIM1, TIM_TS_TI2FP2 );
	TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Trigger );
	TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable );
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Enable );
	TIM_SelectInputTrigger( TIM2, TIM_TS_ITR0 );
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Trigger );
}

/*********************************************************************
 * @fn      Hardware
 *
 *
 * @return  none
 */
void Hardware(void)
{
	ExtTrigger_Start_Two_TIM( 0xFFFF, 48000-1);
	while(1)
	{
		printf("TIM1 cnt:%d\r\n", TIM1->CNT);
		printf("TIM2 cnt:%d\r\n", TIM2->CNT);
		Delay_Ms(100);
	}
}
