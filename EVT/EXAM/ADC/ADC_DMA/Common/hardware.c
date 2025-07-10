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

u16 TxBuf[1024];

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
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_LowPowerModeCmd(ADC1,ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_BufferCmd(ADC1, DISABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, uint32_t memadr, uint16_t bufsize)
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
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure); 
    
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief    ADC DMA sampling.
 *
 * @return  none
 */
void Hardware(void)
{
	u16 i=0;
    ADC_Function_Init();
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_CyclesMode5);
    DMA_Rx_Init(DMA1_Channel1, (int32_t)& ADC1->RDATAR, (uint32_t)TxBuf, 1024);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 0x78); 
    DMA_Cmd(DMA1_Channel1, ENABLE); 
    DMA_Cmd(DMA1_Channel1, ENABLE); 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(DMA_GetFlagStatus(DMA1,DMA1_FLAG_TC1) == RESET) /* Wait until USART2 TX DMA1 Transfer Complete */
    {

    }

     for(i = 0; i < 1024; i++){

         printf("%d\r\n",TxBuf[i]);

        Delay_Ms(10);
    }
	while (1)
		;
}
