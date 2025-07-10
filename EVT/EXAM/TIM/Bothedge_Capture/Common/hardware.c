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


void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_CC_IRQHandler(void)
{
   
	if( TIM_GetITStatus( TIM1, TIM_IT_CC2 ) != RESET )
	{
		printf( "1CH2_Val:%d\r\n", TIM1->CH2CVR_32 );

	}

	 TIM_ClearITPendingBit( TIM1, TIM_IT_CC2 );
}


void Input_TIM1Capture_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_ICInitTypeDef TIM_ICInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOE|RCC_HB2Periph_AFIO|RCC_HB2Periph_TIM1, ENABLE );
	 GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF1);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;
    TIM_ICInit(TIM1, &TIM_ICInitStructure );

	NVIC_SetPriority(TIM1_CC_IRQn,0);
	NVIC_EnableIRQ(TIM1_CC_IRQn);

	TIM_ITConfig( TIM1, TIM_IT_CC2, ENABLE );
	TIM_SelectInputTrigger( TIM1, TIM_TS_TI1F_ED );	
    TIM_IC2BothEdge_Cmd(TIM1, ENABLE);

    // TIM_ICBothEdgeLevelIndication_Cmd(TIM1, ENABLE);

	TIM_Cmd( TIM1, ENABLE );
}


/*********************************************************************
 * @fn      Hardware
 *
 * TIM1_INT
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Bothedge_Capture\r\n");
	Input_TIM1Capture_Init( 0xFFFF, 10000-1 );

	while (1)
		;
}
