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
#include "PD_Process.h"

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
volatile UINT8  Tim_Ms_Cnt = 0x00;
void TIM1_Init( u16 arr, u16 psc )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_TIM1, ENABLE );
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );

	NVIC_SetPriority(TIM1_UP_IRQn, 3);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig( TIM1, TIM_IT_Update, ENABLE );
    TIM_Cmd( TIM1, ENABLE );
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
    #ifdef Core_V3F
	printf( "PD SRC TEST\r\n" );
    PD_Init( );
	TIM1_Init( 999, 48-1);
	while(1)
    {
        /* Get the calculated timing interval value */
        TIM_ITConfig( TIM1, TIM_IT_Update , DISABLE );
        Tmr_Ms_Dlt = Tim_Ms_Cnt - Tmr_Ms_Cnt_Last;
        Tmr_Ms_Cnt_Last = Tim_Ms_Cnt;
        TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );
        PD_Ctl.Det_Timer += Tmr_Ms_Dlt;
        if( PD_Ctl.Det_Timer > 4 )
        {
            PD_Ctl.Det_Timer = 0;
            PD_Det_Proc( );
        }
        PD_Main_Proc( );
    }
    #else
	printf( "PD SRC TEST\r\n" );
    PD_Init( );
	TIM1_Init( 999, 48-1);
	while(1)
    {
        /* Get the calculated timing interval value */
        TIM_ITConfig( TIM1, TIM_IT_Update , DISABLE );
        Tmr_Ms_Dlt = Tim_Ms_Cnt - Tmr_Ms_Cnt_Last;
        Tmr_Ms_Cnt_Last = Tim_Ms_Cnt;
        TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );
        PD_Ctl.Det_Timer += Tmr_Ms_Dlt;
        if( PD_Ctl.Det_Timer > 4 )
        {
            PD_Ctl.Det_Timer = 0;
            PD_Det_Proc( );
        }
        PD_Main_Proc( );
    }
       #endif
}
/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 interrupt.
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void)
{
    if( TIM_GetITStatus( TIM1, TIM_IT_Update ) != RESET )
    {
        Tim_Ms_Cnt++;
        TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
    }
}

