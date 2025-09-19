/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/16
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "string.h"

/* Global define */
#define TIM1_CH2CVR_ADDRESS    0x40012C38
#define Mode1  0
#define Mode2  1
#define Size   3

// #define Mode   Mode1
#define Mode   Mode2

/* Global Variable */
u16 pbuf[Size] = {10, 20, 30};
u16 pbuf1[Size] = {70, 80, 90};
u16 bbuf[Size] = {10, 20, 30};
u16 bbuf1[Size] = {70, 80, 90};

/*********************************************************************
 * @fn      TIM1_PWMOut_Init
 *
 * @brief   Initializes TIM1 PWM output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE | RCC_HB2Periph_TIM1|RCC_HB2Periph_AFIO, ENABLE);
	
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
}

/*********************************************************************
 * @fn      TIM1_DMA_Init
 *
 * @brief   Initializes the TIM DMAy Channelx configuration.
 *
 * @param   DMAy_CHx -
 *            y can be 1 to 2.   
 *            x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory0 base address.
 *          memadr1 - Memory1 base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void TIM1_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr,u32 mem1adr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);

    NVIC_SetPriority(DMA1_Channel1_IRQn,0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr = memadr;

#if(Mode == Mode1)
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
#elif(Mode == Mode2)    
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
#endif

    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_InitStructure.DMA_BufferMode = DMA_DoubleBufferMode;
    DMA_InitStructure.DMA_Memory1BaseAddr = mem1adr;
    DMA_InitStructure.DMA_DoubleBuffer_StartMemory = DMA_DoubleBufferMode_Memory_0;

    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_Cmd(DMA_CHx, ENABLE);
}

/*********************************************************************
 * @fn      DMA_D_Buffer__Mode1
 *
 * @brief   Transfer from peripheral to RAM
 *
 * @param   
 *
 * @return  none
 */
void DMA_D_Buffer_Test_Mode1(void)
{
    u32 t;
    printf("DMA_D_Buf_test\r\n");

    printf("Start Wr\r\n");
    memset(pbuf, 0, sizeof(pbuf));
    memset(pbuf1, 0, sizeof(pbuf1));
    memset(bbuf, 0, sizeof(bbuf));
    memset(bbuf1, 0, sizeof(bbuf1));
    printf("Wr End\r\n");

    printf("Start RD1\r\n");
    for(t=0; t<Size; t++){
        printf("pbuf[%d]=%04x\r\n",t,pbuf[t]);
        printf("pbuf1[%d]=%04x\r\n",t,pbuf1[t]);
        printf("bbuf[%d]=%04x\r\n",t,bbuf[t]);
        printf("bbuf1[%d]=%04x\r\n",t,bbuf1[t]);
    }
    printf("RD End1\r\n");

    TIM1_PWMOut_Init(20 - 1, 10000 - 1, 5);
    TIM1_DMA_Init(DMA1_Channel1, (u32)TIM1_CH2CVR_ADDRESS, (u32)pbuf,(u32)pbuf1, Size);      
    DMA_MuxChannelConfig(DMA_MuxChannel1,5);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    while((DMA1_Channel1->CFGR & (1<<16)) == RESET);
    DMA1_Channel1->MADDR = (u32)&bbuf[0];
    printf("Start RD2\r\n");
    for(t=0; t<Size; t++){
        printf("pbuf[%d]=%04x\r\n",t,pbuf[t]);
        printf("pbuf1[%d]=%04x\r\n",t,pbuf1[t]);
        printf("bbuf[%d]=%04x\r\n",t,bbuf[t]);
        printf("bbuf1[%d]=%04x\r\n",t,bbuf1[t]);
    }
    printf("RD End2\r\n");
  
    while((DMA1_Channel1->CFGR & (1<<16)) != RESET);
    DMA1_Channel1->M1ADDR = (u32)&bbuf1[0];
    printf("Start RD3\r\n");
    for(t=0; t<Size; t++){
        printf("pbuf[%d]=%04x\r\n",t,pbuf[t]);
        printf("pbuf1[%d]=%04x\r\n",t,pbuf1[t]);
        printf("bbuf[%d]=%04x\r\n",t,bbuf[t]);
        printf("bbuf1[%d]=%04x\r\n",t,bbuf1[t]);
    }
    printf("RD End3\r\n");

    while((DMA1_Channel1->CFGR & (1<<16)) == RESET);

    printf("Start RD4\r\n");
    for(t=0; t<Size; t++){
        printf("pbuf[%d]=%04x\r\n",t,pbuf[t]);
        printf("pbuf1[%d]=%04x\r\n",t,pbuf1[t]);
        printf("bbuf[%d]=%04x\r\n",t,bbuf[t]);
        printf("bbuf1[%d]=%04x\r\n",t,bbuf1[t]);
    }
    printf("RD End4\r\n");

    while((DMA1_Channel1->CFGR & (1<<16)) != RESET);

    printf("Start RD5\r\n");
    for(t=0; t<Size; t++){
        printf("pbuf[%d]=%04x\r\n",t,pbuf[t]);
        printf("pbuf1[%d]=%04x\r\n",t,pbuf1[t]);
        printf("bbuf[%d]=%04x\r\n",t,bbuf[t]);
        printf("bbuf1[%d]=%04x\r\n",t,bbuf1[t]);
    }
    printf("RD End5\r\n");
}

/*********************************************************************
 * @fn      DMA_D_Buffer__Mode2
 *
 * @brief   Transfer from RAM to peripheral
 *
 * @param   
 *
 * @return  none
 */
void DMA_D_Buffer_Test_Mode2(void)
{
   	printf("DMA_D_Buf_test\r\n");
    TIM1_PWMOut_Init(100 - 1, 10000 - 1, 60);
    TIM1_DMA_Init(DMA1_Channel1, (u32)TIM1_CH2CVR_ADDRESS, (u32)pbuf,(u32)pbuf1, Size);      
    DMA_MuxChannelConfig(DMA_MuxChannel1,5);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE); 
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{

#if(Mode == Mode1)
    DMA_D_Buffer_Test_Mode1();
#elif(Mode == Mode2)
    DMA_D_Buffer_Test_Mode2();  
#endif

    while(1)
    {

    }
}
