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

/*********************************************************************
 * @fn      TIM8_Dead_Time_Init
 *
 * @brief   Initializes TIM8 complementary output and dead time.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM8_Dead_Time_Init( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC|RCC_HB2Periph_AFIO|RCC_HB2Periph_TIM8, ENABLE );

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF3);

	/* TIM8_CH1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	/* TIM8_CH1N */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init( TIM8, &TIM_OCInitStructure );

    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0xFF;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig( TIM8, &TIM_BDTRInitStructure );

	TIM_CtrlPWMOutputs(TIM8, ENABLE );
	TIM_OC1PreloadConfig( TIM8, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM8, ENABLE );
	TIM_Cmd( TIM8, ENABLE );
}
/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("TIM8_Dead\r\n");
	/* Complementary output with dead-time insertion */
	TIM8_Dead_Time_Init( 100-1, 144-1, 50 );
	/* Dead-time waveforms with delay greater than the negative pulse */
//	TIM8_Dead_Time_Init( 100-1, 144-1, 10 );
	/* Dead-time waveforms with delay greater than the positive pulse. */
//	TIM8_Dead_Time_Init( 100-1, 144-1, 90 );

	while (1)
		;
}
