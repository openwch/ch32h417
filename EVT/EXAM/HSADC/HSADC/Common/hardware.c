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
__attribute__((aligned(32))) u16 txbuf[1024];
__attribute__((aligned(32))) u16 txbuf1[1024];

/*********************************************************************
 * @fn      HSADC_Function_Init
 *
 * @brief   Initializes HSADC collection.
 *
 * @return  none
 */
void  HSADC_Function_Init(void)
{
    HSADC_InitTypeDef  HSADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC|RCC_HB2Periph_HSADC, ENABLE );
    RCC_HSADCCLKConfig(RCC_HSADCSource_PLLCLK);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    HSADC_InitStructure.HSADC_BurstMode = ENABLE;
    HSADC_InitStructure.HSADC_DMA_TransferLen = 255;
    HSADC_InitStructure.HSADC_BurstMode_TransferLen = 3;
    HSADC_InitStructure.HSADC_BurstMode_DMA_LastTransferLen = 16;
    
    HSADC_InitStructure.HSADC_ClockDivision = 3;
    HSADC_InitStructure.HSADC_DMA = ENABLE;
    HSADC_InitStructure.HSADC_RxAddress0 = (u32)txbuf;
    HSADC_InitStructure.HSADC_RxAddress1 = (u32)txbuf1;
    
    HSADC_InitStructure.HSADC_DualBuffer =ENABLE;
    HSADC_InitStructure.HSADC_FirstConversionCycle = HSADC_First_Conversion_Cycle_8;
    HSADC_InitStructure.HSADC_DataSize = HSADC_DataSize_16b;
    HSADC_Init(&HSADC_InitStructure);

    HSADC_BurstEndCmd(ENABLE);
    HSADC_Cmd(ENABLE);
    HSADC_ChannelConfig(HSADC_Channel_0);
    NVIC_EnableIRQ(HSADC_IRQn);
	NVIC_SetPriority(HSADC_IRQn,2);
    HSADC_ITConfig(HSADC_IT_BurstEnd, ENABLE);
 
}
vu8  HS_FLAG=0;
/*********************************************************************
 * @fn      Hardware
 *
 * @brief   HSADC conversion.
 *
 * @return  none
 */
void Hardware(void)
{
    u16 i=0;
    HSADC_Function_Init(); 
    HSADC_SoftwareStartConvCmd(ENABLE);	
    while(HS_FLAG==0)
    {
    } 
    for(i=0;i<512;i++)
    {
        printf("Addre1_dat  %d\r\n",txbuf1[i]);
        Delay_Ms(10);
    }

    for(i=0;i<528;i++)
    {
        printf("Addre0_dat  %d\r\n",txbuf[i]);
        Delay_Ms(10);
    }
	while(1);

}
void HSADC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      HSADC_IRQHandler
 *
 * @brief   HSADC Interrupt Service Function.
 *
 * @return  none
 */
void HSADC_IRQHandler()
{

    if(HSADC_GetITStatus( HSADC_IT_BurstEnd)){
        HS_FLAG=1;  
    }
    HSADC_ClearITPendingBit( HSADC_IT_BurstEnd);

}
