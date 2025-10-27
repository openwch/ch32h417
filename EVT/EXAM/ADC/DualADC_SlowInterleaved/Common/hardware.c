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


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);
    ADC_InitStructure.ADC_Mode = ADC_Mode_SlowInterl;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode1 );

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	if(((DBGMCU_GetCHIPID())&0xF0)==0)
	{
      ADC_HD_CalibrationCmd(ADC1,DISABLE);
	} 

    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_CyclesMode1 );

    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
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
 * @brief     Dual ADC slow interleaved sampling.
 *
 * @return  none
 */
void Hardware(void)
{
    ADC_Function_Init(); 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(1)
	{
       

        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        ADC_val1 = ADC1->RDATAR&0xffff;
        ADC_val2 = (ADC1->RDATAR>>16)&0xffff;

        printf( "ADC1 ch2=%04d\r\n", ADC_val1);
        printf( "ADC2 ch2=%04d\r\n", ADC_val2);
        Delay_Ms(1000);
	}
}
