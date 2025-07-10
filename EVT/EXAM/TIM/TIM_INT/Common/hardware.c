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


/* Mode Definition */
#define MODE1   0
#define MODE2   1

/* Mode Selection */
#define MODE MODE1
//#define MODE MODE2

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 UP exception.
 *
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update)==SET)
    {
        printf("--------updata\r\n");
    }
    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
}

/*********************************************************************
 * @fn      TIM1_INT_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void TIM1_INT_Init( u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 50;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );

	NVIC_SetPriority(TIM1_UP_IRQn,0);
	NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

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
	printf("TIM_INT\r\n");
    TIM1_INT_Init( 200-1, 14400-1);
    #if(MODE==MODE1)

    TIM_Cmd( TIM1, ENABLE );

    #elif(MODE==MODE2)

    TIM_GenerateEvent(TIM1, TIM_IT_Update);

    #endif
	while (1)
		;
}
