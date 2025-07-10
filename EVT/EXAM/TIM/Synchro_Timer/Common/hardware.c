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
 * @fn      TIM_TimSynchroMode1_Init
 *
 * @brief   Using TIM2 as prescaler for TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode1_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x3E8);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_ITRxExternalClockConfig(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_External1);

    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM_TimSynchroMode2_Init
 *
 * @brief   Using TIM2 to use TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode2_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x3E8);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x64;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC1Ref);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);

    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      TIM_TimSynchroMode3_Init
 *
 * @brief   Using TIM2 to start TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode3_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0xFFF);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);
    TIM_Cmd(TIM1, ENABLE);
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
	printf("Synchro_Timer\r\n");
	/* Timer synchronization Mode Selection */
   // TIM_TimSynchroMode1_Init();
   //	TIM_TimSynchroMode2_Init();
    	TIM_TimSynchroMode3_Init();
        // Delay_Ms(500);
	while(1)
	{

        printf("TIM1 cnt:%d\r\n", TIM1->CNT);
        printf("TIM2 cnt:%d\r\n", TIM2->CNT);
	}
	while (1)
		;
}
