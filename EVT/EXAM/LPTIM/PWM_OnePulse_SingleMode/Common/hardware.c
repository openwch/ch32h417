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
/* LPTIM work mode */
#define OnePulseMode             1
#define SingleMode               2
#define PWM                      3

/* LPTIM Trigger Source */
#define ETR                      1//PD11

/* LPTIM work mode select */
//#define MODE       OnePulseMode
//#define MODE       SingleMode
#define MODE       PWM

/* LPTIM Trigger Source select */
#define Trigger    ETR


/*********************************************************************
 * @fn      LPTIM_Init
 *
 * @brief   LPTIM_Init.
 *
 * @return  none
 */
void LPTIM_Init(u16 cmp,u16 arr)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    LPTIM_TimeBaseInitTypeDef   LPTIM_TimeBaseInitStruct = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_LPTIM1, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE|RCC_HB2Periph_GPIOD|RCC_HB2Periph_AFIO, ENABLE);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF1);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    LPTIM_Cmd(LPTIM1,ENABLE);

    LPTIM_TimeBaseInitStruct.LPTIM_ClockSource = LPTIM_ClockSource_In;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPolarity = LPTIM_ClockPolarity_Rising;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSampleTime = LPTIM_ClockSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_TriggerSampleTime = LPTIM_TriggerSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPrescaler = LPTIM_TClockPrescaler_DIV1;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    LPTIM_TimeBaseInitStruct.LPTIM_TriggerSource = LPTIM_TriggerSource_ETR;

    LPTIM_TimeBaseInitStruct.LPTIM_ExTriggerPolarity = LPTIM_ExTriggerPolarity_Rising;
    LPTIM_TimeBaseInitStruct.LPTIM_TimeOut = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_OutputPolarity = LPTIM_OutputPolarity_High;
    LPTIM_TimeBaseInitStruct.LPTIM_UpdateMode = LPTIM_UpdateMode0;
    LPTIM_TimeBaseInitStruct.LPTIM_CountSource = LPTIM_CountSource_Internal;
    LPTIM_TimeBaseInitStruct.LPTIM_Encoder = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_InClockSource = LPTIM_InClockSource_PCLK1;
    LPTIM_TimeBaseInitStruct.LPTIM_ForceOutHigh = DISABLE;
#if(MODE == OnePulseMode)
    LPTIM_TimeBaseInitStruct.LPTIM_OnePulseMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_SingleMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_PWMOut = ENABLE;
#elif(MODE == SingleMode)
    LPTIM_TimeBaseInitStruct.LPTIM_SingleMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_PWMOut = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ContinuousMode = DISABLE;
#elif(MODE == PWM)
    LPTIM_TimeBaseInitStruct.LPTIM_PWMOut = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ContinuousMode = ENABLE;
#endif

    LPTIM_TimeBaseInitStruct.LPTIM_CounterDirIndicat = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_Pulse = cmp;
    LPTIM_TimeBaseInitStruct.LPTIM_Period = arr;

    LPTIM_TimeBaseInit( LPTIM1,& LPTIM_TimeBaseInitStruct);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("LPTIM\r\n");
	LPTIM_Init(50,100);
	while (1)
		;
}
