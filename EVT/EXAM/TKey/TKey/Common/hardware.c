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
TKey1->CTLR1 |= (1 << 26) | (1 << 24); // Enable TouchKey and Buffer


}



/*********************************************************************
 * @fn      Touch_Key_Adc
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
 * @return  val - The Data conversion value.
 */
u16 Touch_Key_Adc(u8 ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_CyclesMode7);
    TKey1->IDATAR1 = 0x9F; //Charging Time
    TKey1->RDATAR = 0xff;   //Discharging Time
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    
    return (uint16_t)TKey1->RDATAR;
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
	u16 ADC_val;
	ADC_Function_Init();	
	while(1)
	{
        ADC_val = Touch_Key_Adc(ADC_Channel_1);
        printf("TouckKey Value:%d\r\n", ADC_val);
        Delay_Ms(100);
	}
}
