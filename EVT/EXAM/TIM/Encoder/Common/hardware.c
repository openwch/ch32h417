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

#define SpeedSampleTimeMs 20
volatile int circle = 0, precircle = 0;
volatile uint16_t precnt = 0;
volatile uint32_t time = 0;

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{

    volatile uint16_t tempcnt = TIM1->CNT, temparr = TIM1->ATRLR;
    if (TIM_GetITStatus(TIM1, TIM_IT_Update))
    {

        if (tempcnt < temparr / 2)
        {
            circle += 1;
        }
        else
        {
            circle -= 1;
        }
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/*********************************************************************
 * @fn TIM1_Encoder_Init
 * 
 * @brief function initializes the TIM1 timer as an encoder with specific settings.
 * 
 * @return none
 */
void TIM1_Encoder_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_ICInitTypeDef TIM_ICInitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE | RCC_HB2Periph_TIM1|RCC_HB2Periph_AFIO, ENABLE );

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF1);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF1);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_Init( GPIOE, &GPIO_InitStructure );

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseStructure.TIM_Period = 80;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

	NVIC_SetPriority(TIM1_UP_IRQn,0);
	NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_SetCounter(TIM1, 0);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn TIM8_EncoderSignalGenerator_Init
 * 
 * @return none
 */
void TIM8_EncoderSignalGenerator_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_TIM8 | RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOC, ENABLE );

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF3);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = 100-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 10000-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init( TIM8, &TIM_OCInitStructure );
    TIM_OCInitStructure.TIM_Pulse = 50;//Phase shift 90
    TIM_OC3Init( TIM8, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM8, ENABLE );
	TIM_OC1PreloadConfig( TIM8, TIM_OCPreload_Disable );
    TIM_OC3PreloadConfig( TIM8, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM8, ENABLE );
	TIM_Cmd( TIM8, ENABLE );
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
    printf("Encoder TEST\r\n");
    TIM8_EncoderSignalGenerator_Init();
    TIM1_Encoder_Init();

    while (1)
    {
        /* The code block you provided is checking if the encoder position or count has changed. If
        there is a change, it prints the new encoder position and calculates the encoder speed. */
        if (precircle != circle || (precnt != TIM1->CNT && TIM1->CNT % 4 == 0))
        {
              printf("Encoder position= %d circle %d step\r\n", circle, TIM1->CNT);
            if (time != 0)
                 printf("Encoder speed= %f\r\n", -(float)(precircle * 80 + precnt - (circle * 80 + TIM1->CNT)) / (float)time * 1000.0/(float)SpeedSampleTimeMs / 80.0);
            else
                printf("Encoder speed null!!\r\n");
            time = 0;
            precircle = circle;
            precnt = TIM1->CNT;
        }
        time++;
        Delay_Ms(SpeedSampleTimeMs);
     }

}
