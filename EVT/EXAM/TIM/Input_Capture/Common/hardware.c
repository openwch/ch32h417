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


void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      TIM8_CC_IRQHandler
 *
 * @brief   This function handles TIM8  Capture Compare Interrupt exception.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{

	if( TIM_GetITStatus( TIM2, TIM_IT_CC1 ) != RESET )
	{
		printf( "CH1_Val:%d\r\n", TIM_GetCapture1( TIM2 ) );
	}

	if( TIM_GetITStatus( TIM2, TIM_IT_CC2 ) != RESET )
	{
		printf( "CH2_Val:%d\r\n", TIM_GetCapture2( TIM2 ) );
	}

	TIM_ClearITPendingBit( TIM2, TIM_IT_CC1 | TIM_IT_CC2 );
}

/*********************************************************************
 * @fn      Input_Capture_Init
 *
 * @brief   Initializes TIM2 input capture.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void Input_Capture_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_ICInitTypeDef TIM_ICInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOB|RCC_HB2Periph_AFIO, ENABLE );
    RCC_HB1PeriphClockCmd( RCC_HB1Periph_TIM2, ENABLE );
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

	TIM_PWMIConfig( TIM2, &TIM_ICInitStructure );

	NVIC_SetPriority(TIM2_IRQn,0);
	NVIC_EnableIRQ(TIM2_IRQn);

	TIM_ITConfig( TIM2, TIM_IT_CC1 | TIM_IT_CC2, ENABLE );

	TIM_SelectInputTrigger( TIM2, TIM_TS_TI1FP1 );
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Reset );
	TIM_SelectMasterSlaveMode( TIM2, TIM_MasterSlaveMode_Enable );
	TIM_Cmd( TIM2, ENABLE );
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Input_Capture.
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Input_Capture\r\n");
    Input_Capture_Init( 0xFFFF, 14400-1 );
	while (1)
		;
}
