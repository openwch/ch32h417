/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32h417_crc.c
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

#define FlT_Sinc_Order      DFSDM_FLT_Sinc2
#define FLT_Over_Sample     32
#define Right_Bit_Shift     8


/*********************************************************************
 * @fn      TIM2_Function_Init
 *
 * @brief   Initializes TIM_TRGO.
 *
 * @return  none
 */
void TIM2_Function_Init(u16 arr, u16 psc, u16 ccp)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}

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

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC2|RCC_HB2Periph_GPIOA, ENABLE );
    RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
    RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV4,RCC_HCLK_ADCPRE_DIV8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	ADC_DeInit(ADC2);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC2, &ADC_InitStructure);

	/*enable sample data t0 DFSDM */
	ADC_TO_DFSDM_Cmd(ADC2, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
    
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
    
    ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 1, ADC_SampleTime_CyclesMode7 );
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

/*********************************************************************
 * @fn      DFSDM_Function_Init
 *
 * @brief   Initializes DFSDM collection.
 *
 * @return  none
 */
void DFSDM_Function_Init(void)
{
    DFSDM_ChannelInitTypeDef DFSDM_ChannelInitStructure = {0};
    DFSDM_FilterInitTypeDef DFSDM_FilterInitStructure = {0};
    DFSDM_JcInitTypeDef DFSDM_JcInitStructure = {0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_DFSDM, ENABLE);

    /* initialize the parameters of DFSDM */
    DFSDM_ChannelStructInit(&DFSDM_ChannelInitStructure);
    DFSDM_FilterStructInit(&DFSDM_FilterInitStructure);
    DFSDM_JcStructInit(&DFSDM_JcInitStructure);

    /* initialize DFSDM channel 1 */
    DFSDM_ChannelInitStructure.DFSDM_ChAWDSincFilterOrder = DFSDM_AWD_FastSinc;
    DFSDM_ChannelInitStructure.DFSDM_ChAWDFilterOverSample = DFSDM_AWD_FLT_Bypass;
    DFSDM_ChannelInitStructure.DFSDM_ChDataPackMode = DFSDM_StandardMode;
    DFSDM_ChannelInitStructure.DFSDM_ChDataMultiplexer = DFSDM_ADCInput;
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectCurrent;
    DFSDM_ChannelInitStructure.DFSDM_ChCalibrationOffset = 0;
    DFSDM_ChannelInitStructure.DFSDM_ChDataRightBitShift = Right_Bit_Shift;
    DFSDM_ChannelInit(DFSDM_Channel1, &DFSDM_ChannelInitStructure);

    /* initialize DFSDM filter 0 */
    DFSDM_FilterInitStructure.DFSDM_FltSincOrder = FlT_Sinc_Order;
    DFSDM_FilterInitStructure.DFSDM_FltOverSample = FLT_Over_Sample;
    DFSDM_FilterInitStructure.DFSDM_FltIntegratorOverSample = 1;
    DFSDM_FilterInit(DFSDM_FLT0, &DFSDM_FilterInitStructure);

    /* initialize DFSDM filter 0 injected conversions */
	DFSDM_JcInitStructure.DFSDM_JcChannelGroup = DFSDM_JC_Channel1;
	DFSDM_JcInitStructure.DFSDM_JcDMAMode = DISABLE;
	DFSDM_JcInitStructure.DFSDM_JcSynchronousMode = DISABLE;
	DFSDM_JcInitStructure.DFSDM_JcScanMode = DISABLE;
	DFSDM_JcInitStructure.DFSDM_JcTriggerEdge = DFSDM_JC_Trigger_Rising;
	DFSDM_JcInitStructure.DFSDM_JcTriggerSignal = DFSDM_JC_Trigger_TIM2;
    DFSDM_JcInit(DFSDM_FLT0, &DFSDM_JcInitStructure);
 
    /* enable DFSDM channel 0 */
    DFSDM_ChannelCmd(DFSDM_Channel1, ENABLE);
    /* enable DFSDM filter 0 */
    DFSDM_FilterCmd(DFSDM_FLT0, ENABLE);
    /* enable DFSDM interface */
    DFSDM_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	uint32_t value = 0;

    TIM2_Function_Init(9600,10000-1,4800);
    ADC_Function_Init();
    DFSDM_Function_Init();

    while(1)
    {
        if(DFSDM_GetFlagStatus(DFSDM_FLT0, DFSDM_FLAG_FLTx_JEOCF) == SET)
        {
            value = DFSDM_JcReadConvData(DFSDM_FLT0);
            printf("conversion value = %d \r\n",value);
        }       
    }

}
