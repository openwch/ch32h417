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
u16 ADC_val1,ADC_val2;

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

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_ADC2|RCC_HB2Periph_GPIOA, ENABLE );
	RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV4,RCC_HCLK_ADCPRE_DIV8);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegInjecSimult;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStructure);

	ADC_LowPowerModeCmd(ADC1,ENABLE);
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);  
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_CyclesMode4 );
    ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_BufferCmd(ADC1, DISABLE);
    if(((DBGMCU_GetCHIPID())&0xF0)==0)
	{
      ADC_HD_CalibrationCmd(ADC1,DISABLE);
	} 
    
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_LowPowerModeCmd(ADC2,ENABLE);
	ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_None);  
	ADC_InjectedSequencerLengthConfig(ADC2, 1);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 1, ADC_SampleTime_CyclesMode4 );
    ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Dual ADC injection simultaneous sampling.
 *
 * @return  none
 */
void Hardware(void)
{
    ADC_Function_Init(); 

	while(1)
	{
       
	    ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
        Delay_Ms(100);

        ADC_SoftwareStartInjectedConvCmd(ADC1, DISABLE);
        Delay_Ms(100);

        while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_JEOC ));
        ADC_val1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_val2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);

        printf( "JADC1 ch1=%04d\r\n", ADC_val1);
        printf( "JADC2 ch3=%04d\r\n", ADC_val2);
        Delay_Ms(1000);
	}
}
