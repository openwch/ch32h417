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

#define BufferSize             8192
#define Play_BufferSize        16384

#define FLT_SincOrder          DFSDM_FLT_Sinc3 
#define FLT_IntegrOverSample   0
#define DataRightBitShift      2    
#define SCDCntthreshold        255 

#define FLT_OverSample         128
#define ClkoutDiv              195


/* limit the range of stereo data */
#define LimitData(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/* PDM left channel data */
__attribute__((aligned(32))) int32_t PDM_LeftBuffer[BufferSize];
/* PDM right channel data */
__attribute__((aligned(32))) int32_t PDM_RightBuffer[BufferSize];
/* audio data  */
__attribute__((aligned(32))) int16_t PDM_PlayBuffer[Play_BufferSize] = {0};

uint32_t TxCnt = 0;

/*********************************************************************
 * @fn      DFSDM_Function_Init
 *
 * @brief   Initializes DFSDM GPIO collection.
 *
 * @return  none
 */
void DFSDM_I2S_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE);

    /* Config DFSDM_CKOUT(PC2) -- DFSDM_DATIN1(PC3)  */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF6); 
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* WS PB12(AF5) */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* CK PB13(AF5) */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SD PC1(AF5) */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
      
    /* MCK PC6(AF5) */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      I2S1_Init
 *
 * @brief   Initializes I2S1 collection.
 *
 * @return  none
 */
void I2S1_Init(void)
{
    I2S_InitTypeDef  I2S_InitStructure = {0};
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI2, ENABLE);

    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_16k;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_InitStructure.I2S_Standard = I2S_Standard_MSB;
    I2S_Init(SPI2, &I2S_InitStructure);

    NVIC_EnableIRQ(SPI2_IRQn);
    NVIC_SetPriority(SPI2_IRQn, 1);

    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    I2S_Cmd(SPI2, ENABLE);

}

/*********************************************************************
 * @fn      DMA_Function_Init
 *
 * @brief   Initializes DMA collection.
 *
 * @return  none
 */
void DMA_Function_Init(void) 
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (int32_t)& DFSDM_FLT0->RDATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)PDM_RightBuffer;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 107); 
    DMA_Cmd(DMA1_Channel1, ENABLE);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (int32_t)& DFSDM_FLT1->RDATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)PDM_LeftBuffer;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);  
    DMA_MuxChannelConfig(DMA_MuxChannel2, 108); 
    DMA_Cmd(DMA1_Channel2, ENABLE);

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
    DFSDM_RcInitTypeDef DFSDM_RcInitStructure = {0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_DFSDM, ENABLE);

    /* Configure output serial clock source and divider */
    DFSDM_OutSerialClkConfig(DFSDM_AudioClk, ClkoutDiv);

    /* initialize the parameters of DFSDM */
    DFSDM_ChannelStructInit(&DFSDM_ChannelInitStructure);
    DFSDM_FilterStructInit(&DFSDM_FilterInitStructure);
    DFSDM_RcStructInit(&DFSDM_RcInitStructure);

    /* initialize DFSDM channel 0 */
    DFSDM_ChannelInitStructure.DFSDM_ChAWDSincFilterOrder = DFSDM_AWD_FastSinc;
    DFSDM_ChannelInitStructure.DFSDM_ChAWDFilterOverSample = DFSDM_AWD_FLT_Bypass;
    DFSDM_ChannelInitStructure.DFSDM_ChSPIClockSource = DFSDM_InternalClkOut;
    DFSDM_ChannelInitStructure.DFSDM_ChSerialInterface = DFSDM_SPIFalling;
    DFSDM_ChannelInitStructure.DFSDM_ChCalibrationOffset = 0;
    DFSDM_ChannelInitStructure.DFSDM_ChDataPackMode = DFSDM_StandardMode;
    DFSDM_ChannelInitStructure.DFSDM_ChDataMultiplexer = DFSDM_SerialInput;
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectNext;
    DFSDM_ChannelInitStructure.DFSDM_ChShortCircuitDetMode = ENABLE;
    DFSDM_ChannelInitStructure.DFSDM_ChSCDCntthreshold = SCDCntthreshold;
    DFSDM_ChannelInitStructure.DFSDM_ChDataRightBitShift = DataRightBitShift;
    DFSDM_ChannelInit(DFSDM_Channel0, &DFSDM_ChannelInitStructure);  

    /* initialize DFSDM channel 1 */
    DFSDM_ChannelInitStructure.DFSDM_ChAWDSincFilterOrder = DFSDM_AWD_FastSinc;
    DFSDM_ChannelInitStructure.DFSDM_ChAWDFilterOverSample = DFSDM_AWD_FLT_Bypass;
    DFSDM_ChannelInitStructure.DFSDM_ChSPIClockSource = DFSDM_InternalClkOut;
    DFSDM_ChannelInitStructure.DFSDM_ChSerialInterface = DFSDM_SPIRising;  
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectCurrent; 
    DFSDM_ChannelInitStructure.DFSDM_ChSCDCntthreshold = SCDCntthreshold; 
    DFSDM_ChannelInitStructure.DFSDM_ChDataRightBitShift = DataRightBitShift; 
    DFSDM_ChannelInit(DFSDM_Channel1, &DFSDM_ChannelInitStructure);  

    /* initialize DFSDM filter 0 and filter 1 */ 
    DFSDM_FilterInitStructure.DFSDM_FltSincOrder = FLT_SincOrder;
    DFSDM_FilterInitStructure.DFSDM_FltOverSample = FLT_OverSample;
    DFSDM_FilterInitStructure.DFSDM_FltIntegratorOverSample = FLT_IntegrOverSample;
    DFSDM_FilterInit(DFSDM_FLT0, &DFSDM_FilterInitStructure);
    DFSDM_FilterInit(DFSDM_FLT1, &DFSDM_FilterInitStructure);  

    /* initialize DFSDM filter 0 regular conversions */
    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel0;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT0, &DFSDM_RcInitStructure);

    /* initialize DFSDM filter 1 regular conversions */
    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel1;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT1, &DFSDM_RcInitStructure);
    
    /* enable DFSDM channel 0 and channel 1*/
    DFSDM_ChannelCmd(DFSDM_Channel0, ENABLE);
    DFSDM_ChannelCmd(DFSDM_Channel1, ENABLE);

    /* enable DFSDM filter 0 and filter 1 */
    DFSDM_FilterCmd(DFSDM_FLT0, ENABLE);
    DFSDM_FilterCmd(DFSDM_FLT1, ENABLE);

    /* enable DFSDM interface */
    DFSDM_Cmd(ENABLE);

    /* enable regular channel conversion by software */
    DFSDM_RcSoftStartConversion(DFSDM_FLT0);
    DFSDM_RcSoftStartConversion(DFSDM_FLT1);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	uint32_t i = 0;

    DFSDM_I2S_GpioInit();
    DMA_Function_Init();
    I2S1_Init();
    DFSDM_Function_Init();
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);

    while(1)
    {
        /* wait for DMA half-full transmit complete */
        while(RESET == DMA_GetFlagStatus(DMA1, DMA1_FLAG_HT1));
        while(RESET == DMA_GetFlagStatus(DMA1, DMA1_FLAG_HT2));

        /* get the PCM stereo data */
        for(i = 0; i < BufferSize / 2; i++) {
            PDM_PlayBuffer[2 * i] = LimitData((PDM_LeftBuffer[i] >> 8), -32768, 32767);
            PDM_PlayBuffer[(2 * i) + 1] = LimitData((PDM_RightBuffer[i] >> 8), -32768, 32767);
        }
        /* clear the half transfer finish flag */
        DMA_ClearFlag(DMA1, DMA1_FLAG_HT1);
        DMA_ClearFlag(DMA1, DMA1_FLAG_HT2);

        /* wait for DMA full transmit complete */
        while(RESET == DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC1));
        while(RESET == DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC2));

        /* get the PCM stereo data */
        for(i = BufferSize / 2; i < BufferSize; i++) {
            PDM_PlayBuffer[2 * i] = LimitData((PDM_LeftBuffer[i] >> 8), -32768, 32767);
            PDM_PlayBuffer[(2 * i) + 1] = LimitData((PDM_RightBuffer[i] >> 8), -32768, 32767);
        }
        /* clear the full transfer finish flag */
        DMA_ClearFlag(DMA1, DMA1_FLAG_TC1);
        DMA_ClearFlag(DMA1, DMA1_FLAG_TC2);        
    }

}


/*********************************************************************
 * @fn      SPI2_IRQHandler
 *
 * @brief   This function handles SPI2 exception.
 *
 * @return  none
 */
void SPI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI2_IRQHandler(void)
{
    if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
    {
        if(TxCnt >= Play_BufferSize)
        {
            TxCnt = 0;
        }
        SPI_I2S_SendData(SPI2, PDM_PlayBuffer[TxCnt]);
        TxCnt++;
    }
}