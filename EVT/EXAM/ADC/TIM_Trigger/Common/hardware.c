/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.5
* Date               : 2026/10/16
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
/* ADC Conversion Mode Definition*/
#define ADC_trig_Mode     0
#define ADC_inje_Mode     1

/* ADC Conversion Mode Selection*/
#define ADC_MODE_Conversion   ADC_trig_Mode
// #define ADC_MODE_Conversion   ADC_inje_Mode



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
	RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV2,RCC_HCLK_ADCPRE_DIV8);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
#else
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
#endif

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_LowPowerModeCmd(ADC1,ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_LowPowerModeCmd(ADC1,ENABLE);

    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_CyclesMode5);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_CC1);  
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
#else
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
#endif

    NVIC_EnableIRQ(ADC1_2_IRQn);
	NVIC_SetPriority(ADC1_2_IRQn,0);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
#else
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
#endif


	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_BufferCmd(ADC1, DISABLE);
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
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
#else
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
#endif
 
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
#else
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
#endif
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

#if Func_Run_V3F

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
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
        ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);  
        #if 0
        printf("ADC Extline trigger inje conversion...\r\n");
        printf("JADC %04d\r\n", ADC_val);
        #endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);

#else
    if(ADC_GetITStatus( ADC1, ADC_IT_EOC)){
        ADC_val = ADC_GetConversionValue(ADC1);
        #if 0
        printf("ADC Extline trigger conversion...\r\n");
        printf("ADC %04d\r\n", ADC_val);
        #endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_EOC);
#endif
}

#endif