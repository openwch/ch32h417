/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/16
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
u16 ADC_val;
/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure={0};
	GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_GPIOA, ENABLE );
	RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV4,RCC_HCLK_ADCPRE_DIV8);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_LowPowerModeCmd(ADC1,ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_LowPowerModeCmd(ADC1,ENABLE);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_CC1);  
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_CyclesMode5);
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

    NVIC_EnableIRQ(ADC1_2_IRQn);
	NVIC_SetPriority(ADC1_2_IRQn,0);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_BufferCmd(ADC1, DISABLE);
}

/*********************************************************************
 * @fn      Get_ADC_Val
 *
 * @brief   Returns ADCx conversion result data.
 *
 * @param   ch - ADC channel.
 *            ADC_Channel_0 - ADC Channel0 selected.
 *            ADC_Channel_1 - ADC Channel1 selected.
 *            ADC_Channel_2 - ADC Channel2 selected.
 *            ADC_Channel_3 - ADC Channel3 selected.
 *            ADC_Channel_4 - ADC Channel4 selected.
 *            ADC_Channel_5 - ADC Channel5 selected.
 *            ADC_Channel_6 - ADC Channel6 selected.
 *            ADC_Channel_7 - ADC Channel7 selected.
 *            ADC_Channel_8 - ADC Channel8 selected.
 *            ADC_Channel_9 - ADC Channel9 selected.
 *            ADC_Channel_10 - ADC Channel10 selected.
 *            ADC_Channel_11 - ADC Channel11 selected.
 *            ADC_Channel_12 - ADC Channel12 selected.
 *            ADC_Channel_13 - ADC Channel13 selected.
 *            ADC_Channel_14 - ADC Channel14 selected.
 *            ADC_Channel_15 - ADC Channel15 selected.
 *            ADC_Channel_16 - ADC Channel16 selected.
 *            ADC_Channel_17 - ADC Channel17 selected.
 *
 * @return  none
 */
u16 Get_ADC_Val(u8 ch)
{
    u16 val;
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_CyclesMode5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	val = ADC_GetConversionValue(ADC1);

	return val;
}
/*********************************************************************
 * @fn      TIM1_PWM_In
 *
 * @brief   Initializes PWM.
 *
 * @return  none
 */

void TIM1_PWM_In(u16 arr, u16 psc, u16 ccp)
{
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   ADC TIM_Trigger.
 *
 * @return  none
 */
void Hardware(void)
{

	TIM1_PWM_In(7200,1000-1,3600);
    ADC_Function_Init();  
}


void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   ADC1 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_2_IRQHandler()
{
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
        printf("ADC Extline trigger conversion...\r\n");
        ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        printf("JADC %04d\r\n", ADC_val);
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
 
}
