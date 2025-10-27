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

u32 TxBuf[1];
u16 Adc_Val[2];
u16 ADC_Val1,ADC_Val2;
vu8 Injected_IT_Flag,DMA_IT_Flag;

void ADC1_2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
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


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult_FastInterl;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);  
	ADC_LowPowerModeCmd(ADC1,ENABLE);
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_CyclesMode5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_CyclesMode5 );
    ADC_DiscModeChannelCountConfig( ADC1, 1);
	ADC_InjectedDiscModeCmd(ADC1 , ENABLE);
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
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
    ADC_InjectedSequencerLengthConfig(ADC2, 1);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 1, ADC_SampleTime_CyclesMode5 );
    ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, ADC_SampleTime_CyclesMode5 );
    ADC_ExternalTrigInjectedConvConfig(ADC2,ADC_ExternalTrigInjecConv_None );
    ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
	NVIC_EnableIRQ(ADC1_2_IRQn);
	NVIC_SetPriority(ADC1_2_IRQn,0);
    ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer

    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
}

void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, uint32_t memadr, uint16_t bufsize)
{
 DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure); 
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	NVIC_SetPriority(DMA1_Channel1_IRQn,2);
	DMA_ITConfig( DMA1_Channel1, DMA_IT_TC , ENABLE );
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

	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	Delay_Ms(1000);
	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
    ADC_Function_Init(); 
	DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf, 1 );
	DMA_Cmd( DMA1_Channel1, ENABLE );
	DMA_MuxChannelConfig(DMA_MuxChannel1, 0x78); 

	while(1)
	{

	    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        ADC_SoftwareStartConvCmd(ADC2, ENABLE);
        ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
        ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);
         
        Delay_Ms(10);

        if(Injected_IT_Flag==1)
        {
            Injected_IT_Flag=0;
            printf( "JADC1 ch0=%04d\r\n", ADC_Val1);
            printf( "JADC2 ch3=%04d\r\n", ADC_Val2);
        }

        if(DMA_IT_Flag==1)
        {
            DMA_IT_Flag=0;
            printf("ADC1 ch5=%d\r\n",Adc_Val[0]);
            printf("ADC2 ch4=%d\r\n",Adc_Val[1]);
        }
        Delay_Ms(1000);
	}
}
/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   This function handles ADC1_2 exception.
 *
 * @return  none
 */
void ADC1_2_IRQHandler()
{
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
        Injected_IT_Flag=1;

        ADC_Val1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_Val2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
    }
    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
    ADC_ClearITPendingBit( ADC2, ADC_IT_JEOC);
}

/*********************************************************************
 * @fn      DMA1_Channel1_IRQHandler
 *
 * @brief   This function handles DMA1 Channel1 exception.
 *
 * @return  none
 */
void DMA1_Channel1_IRQHandler()
{

    if(DMA_GetITStatus(DMA1,DMA1_IT_TC1)==SET ){
        DMA_IT_Flag=1;
        DMA_ClearITPendingBit(DMA1,DMA1_IT_TC1);

        Adc_Val[0]=TxBuf[0]&0xffff;
        Adc_Val[1]=(TxBuf[0]>>16)&0xffff;
    }
}
