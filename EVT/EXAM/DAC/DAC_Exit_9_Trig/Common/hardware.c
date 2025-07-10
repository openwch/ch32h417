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

/* Global define */
#define Num    7

/* Global Variable */
u16 DAC_Value[Num] = {64, 128, 256, 512, 1024, 2048, 4095};

/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dac_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef  DAC_InitType = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_DAC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  

    DAC_InitType.DAC_Trigger = DAC_Trigger_Ext_IT9;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);

    DAC_Cmd(DAC_Channel_1, ENABLE);
}

/*********************************************************************
 * @fn      EXTI_Event_Init
 *
 * @brief   Initializes EXTI collection.
 *
 * @return  none
 */
void EXTI_Event_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);

    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   External trigger DAC conversion.
 *
 * @return  none
 */
void Hardware(void)
{
	u8 i = 0;
    printf("Timer Trig\r\n");

    EXTI_Event_Init();
    Dac_Init();

    while(1)
    {
        DAC->R12BDHR1 = DAC_Value[i];
        i++;
        if(i > Num)
        {
            i = 0;
        }
        Delay_Ms(1000);

        printf("run\r\n");
        printf("DAC->R12BDHR1:0x%04x\r\n", DAC->R12BDHR1);
        printf("DAC->DOR1:0x%04x\r\n", DAC->DOR1);
    }
}
