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
#define Num 7

/* Global Variable */ 
u16 DAC_Value[Num]={64,128,256,512,1024,2048,4095};  
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

	DAC_InitType.DAC_Trigger=DAC_Trigger_T4_TRGO;
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_Triangle;
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_4095;
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;
    DAC_Init(DAC_Channel_1,&DAC_InitType);
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_2047;
    DAC_Init(DAC_Channel_2,&DAC_InitType);

    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);

    DAC_SetDualChannelData(DAC_Align_12b_R, 0,0);
}

/*********************************************************************
 * @fn      Timer4_Init
 *
 * @brief   Initializes TIM4
 *
 * @return  none
 */
void Timer4_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 3600-1;
    TIM_TimeBaseStructure.TIM_Prescaler =0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
    TIM_Cmd(TIM4, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Dual DAC output triangular.
 *
 * @return  none
 */
void Hardware(void)
{
     u8 i=0;
    printf("Noise Wava Generation Test\r\n");
    Dac_Init();
    Timer4_Init();

    while(1)
    {
        DAC->R12BDHR1=DAC_Value[i];
        i++;

        if(i>Num){
          i=0;
        }

        Delay_Ms(1000);
        printf("run\r\n");
        printf("DAC->R12BDHR1:0x%04x\r\n",DAC->R12BDHR1);
        printf("DAC->DOR1:0x%04x\r\n",DAC->DOR1);
        printf("TIM4->CNT:%d\r\n",TIM4->CNT);


    }
}
