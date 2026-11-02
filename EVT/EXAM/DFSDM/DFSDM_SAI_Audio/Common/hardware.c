/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/14
* Description        : This file provides all the hardware firmware functions.
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

#define SAMPLE_RATE (16000)

/* limit the range of stereo data */
#define LimitData(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/* PDM left channel data */
__attribute__((aligned(32))) int32_t PDM_LeftBuffer[BufferSize];
/* PDM right channel data */
__attribute__((aligned(32))) int32_t PDM_RightBuffer[BufferSize];
/* audio data  */
__attribute__((aligned(32))) int16_t PDM_PlayBuffer[Play_BufferSize] = {0};

volatile uint32_t TxCnt = 0;

/*********************************************************************
 * @fn      DFSDM_SAI_GpioInit
 *
 * @brief   Initializes DFSDM GPIO collection.
 *
 * @return  none
 */
void DFSDM_SAI_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC | RCC_HB2Periph_GPIOE, ENABLE);

    /* Config DFSDM_CKOUT(PB0) -- DFSDM_DATIN1(PB12)  */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF6); 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF6);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // SAI_FS_A PC3(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SAI_SCK_A PC2(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SAI_SD_A PB2(AF6)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource2, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SAI_MCLK_A PE2(AF6)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      SAI_Config
 * 
 * @brief   Configure the SAI.
 * 
 * @param   SampleRate - the sampling rate of the SAI.
 * 
 * @return  none
 */
void SAI_Config(uint32_t SampleRate)
{

    SAI_InitTypeDef      SAI_InitStructure      = {0};
    SAI_FrameInitTypeDef SAI_FrameInitStructure = {0};
    SAI_SlotInitTypeDef  SAI_SlotInitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_SAI, ENABLE);

    /* The SAI Clock configuration is calculated as follows:
    SAI_CK_x  = SysClk / 6
    MCLK_x = SAI_CK_x / MCKDIV[5:0] with MCLK_x = 256 * FS
    FS = SAI_CK_x / (MCKDIV[5:0] * 256)
    MCKDIV[5:0] = (SysClk / 6) / (FS * 256) */

    RCC_ClocksTypeDef RCC_ClocksStatus = {0};
    RCC_GetClocksFreq(&RCC_ClocksStatus);

    const uint32_t tmpdiv = (RCC_ClocksStatus.SYSCLK_Frequency / 6) / (SampleRate * 256);

    SAI_InitStructure.SAI_NoDivider     = SAI_MasterDivider_Enabled;
    SAI_InitStructure.SAI_MasterDivider = tmpdiv;
    SAI_InitStructure.SAI_AudioMode     = SAI_Mode_MasterTx;
    SAI_InitStructure.SAI_Protocol      = SAI_Free_Protocol;
    SAI_InitStructure.SAI_DataSize      = SAI_DataSize_16b;
    SAI_InitStructure.SAI_FirstBit      = SAI_FirstBit_MSB;
    SAI_InitStructure.SAI_ClockStrobing = SAI_ClockStrobing_RisingEdge;
    SAI_InitStructure.SAI_Synchro       = SAI_Asynchronous;
    SAI_InitStructure.SAI_FIFOThreshold = SAI_FIFOThreshold_HalfFull;
    SAI_Init(SAI_Block_A, &SAI_InitStructure);

    SAI_FrameInitStructure.SAI_FrameLength       = 32;
    SAI_FrameInitStructure.SAI_ActiveFrameLength = 16;
    SAI_FrameInitStructure.SAI_FSDefinition      = SAI_FS_StartFrame;
    SAI_FrameInitStructure.SAI_FSPolarity        = SAI_FS_ActiveLow;
    SAI_FrameInitStructure.SAI_FSOffset          = SAI_FS_FirstBit;
    SAI_FrameInit(SAI_Block_A, &SAI_FrameInitStructure);

    SAI_SlotInitStructure.SAI_FirstBitOffset = 0;
    SAI_SlotInitStructure.SAI_SlotSize       = SAI_SlotSize_DataSize;
    SAI_SlotInitStructure.SAI_SlotNumber     = 2;
    SAI_SlotInitStructure.SAI_SlotActive     = SAI_SlotActive_ALL;
    SAI_SlotInit(SAI_Block_A, &SAI_SlotInitStructure);

    SAI_FlushFIFO(SAI_Block_A);

    NVIC_EnableIRQ(SAI_IRQn);
    NVIC_SetPriority(SAI_IRQn, 1);

    SAI_Cmd(SAI_Block_A, ENABLE);
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

    DFSDM_SAI_GpioInit();
    DMA_Function_Init();
    SAI_Config(SAMPLE_RATE);
    DFSDM_Function_Init();
    /* enable the SAI interrupt */
    SAI_ITConfig(SAI_Block_A, SAI_IT_FREQ, ENABLE);

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
 * @fn      SAI_IRQHandler
 *
 * @brief   This function handles SAI exception.
 *
 * @return  none
 */
void SAI_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SAI_IRQHandler(void)
{
    if(SAI_GetITStatus(SAI_Block_A, SAI_IT_FREQ) == SET)
    {
        if(TxCnt >= Play_BufferSize)
        {
            TxCnt = 0;
        }
        SAI_SendData(SAI_Block_A, PDM_PlayBuffer[TxCnt]);
        SAI_SendData(SAI_Block_A, PDM_PlayBuffer[TxCnt + 1]);
        SAI_SendData(SAI_Block_A, PDM_PlayBuffer[TxCnt + 2]);
        SAI_SendData(SAI_Block_A, PDM_PlayBuffer[TxCnt + 3]);
        TxCnt += 4;
    }
}

