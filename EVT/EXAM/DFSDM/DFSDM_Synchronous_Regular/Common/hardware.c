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

#define FlT_Sinc_Order    DFSDM_FLT_Sinc3
#define FLT_Over_Sample   32

#define RxBuffSize        20
#define TxbuffSize        ((FLT_Over_Sample * ( 2 + FlT_Sinc_Order + 1) + 2) * RxBuffSize)


int16_t Txdata[TxbuffSize] = {0};

int32_t RxdataFlt0[RxBuffSize] = {0};
int32_t RxdataFlt1[RxBuffSize] = {0};
int32_t Rxdata_buff[RxBuffSize] = {0};

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the DFSDM filter 0 DMA Channelx configuration.
 *
 * @param   DMA_CHx - Select the DMA channel.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize)
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
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);  
}

/*********************************************************************
 * @fn      Internal_Data
 *
 * @brief   Initializes the data.
 *
 * @return  none
 */
void Internal_Data(void)
{
    uint16_t i = 0;
    int8_t m = -10;
    for(i = 0; i < TxbuffSize; i++)
    {
        Txdata[i] = m;
        if((i+1)%(TxbuffSize/RxBuffSize) == 0) m++;
    }
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

    /* initialize the parameters of DFSDM */
    DFSDM_ChannelStructInit(&DFSDM_ChannelInitStructure);
    DFSDM_FilterStructInit(&DFSDM_FilterInitStructure);
    DFSDM_RcStructInit(&DFSDM_RcInitStructure);

    /* initialize DFSDM channel 0 */
    DFSDM_ChannelInitStructure.DFSDM_ChAWDSincFilterOrder = DFSDM_AWD_FastSinc;
    DFSDM_ChannelInitStructure.DFSDM_ChAWDFilterOverSample = DFSDM_AWD_FLT_Bypass;
    DFSDM_ChannelInitStructure.DFSDM_ChDataPackMode = DFSDM_InterleaveMode;
    DFSDM_ChannelInitStructure.DFSDM_ChDataMultiplexer = DFSDM_InternalInput;
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectCurrent;
    DFSDM_ChannelInitStructure.DFSDM_ChDataRightBitShift = 8;
    DFSDM_ChannelInit(DFSDM_Channel0, &DFSDM_ChannelInitStructure);
    DFSDM_ChannelInit(DFSDM_Channel1, &DFSDM_ChannelInitStructure);

    /* initialize DFSDM filter 0 */
    DFSDM_FilterInitStructure.DFSDM_FltSincOrder = DFSDM_FLT_Sinc3;
    DFSDM_FilterInitStructure.DFSDM_FltOverSample = FLT_Over_Sample;
    DFSDM_FilterInitStructure.DFSDM_FltIntegratorOverSample = 1;
    DFSDM_FilterInit(DFSDM_FLT0, &DFSDM_FilterInitStructure);
    DFSDM_FilterInit(DFSDM_FLT1, &DFSDM_FilterInitStructure);

    /* initialize DFSDM filter0 regular conversions */
    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel0;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcSynchronousMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT0, &DFSDM_RcInitStructure);

    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel1;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcSynchronousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT1, &DFSDM_RcInitStructure);

    /* enable DFSDM channel0 */
    DFSDM_ChannelCmd(DFSDM_Channel0, ENABLE);
    DFSDM_ChannelCmd(DFSDM_Channel1, ENABLE);
    /* enable DFSDM FLT0 */
    DFSDM_FilterCmd(DFSDM_FLT0, ENABLE);
    DFSDM_FilterCmd(DFSDM_FLT1, ENABLE);
    /* enable DFSDM interface */
    DFSDM_Cmd(ENABLE);
    /* enable regular channel conversion by software */
    DFSDM_RcSoftStartConversion(DFSDM_FLT0);
}
/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	uint16_t i = 0;

	Internal_Data();

    DMA_Rx_Init(DMA1_Channel1, (int32_t)& DFSDM_FLT0->RDATAR, (int32_t)RxdataFlt0, RxBuffSize);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 107); 
    DMA_Rx_Init(DMA1_Channel2, (int32_t)& DFSDM_FLT1->RDATAR, (int32_t)RxdataFlt1, RxBuffSize);
    DMA_MuxChannelConfig(DMA_MuxChannel2, 108); 
    DFSDM_Function_Init();
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel2, ENABLE);

    for(i = 0; i < TxbuffSize/2; i++)
    { 
        while(DFSDM_GetFlagStatus(DFSDM_FLT0, DFSDM_FLAG_FLTx_RCIP) == RESET); 
        /* Write the parallel data on interleaved mode of data packing */
        DFSDM_WriteParallelDataIntlMode(DFSDM_Channel0, Txdata[2 * i], Txdata[(2 * i) + 1]); 

        while(DFSDM_GetFlagStatus(DFSDM_FLT1, DFSDM_FLAG_FLTx_RCIP) == RESET); 
        /* Write the parallel data on interleaved mode of data packing */
        DFSDM_WriteParallelDataIntlMode(DFSDM_Channel1, Txdata[2 * i], Txdata[(2 * i) + 1]);   
    }

    /* Wait until DFSDM filter0 and filter 1 RX DMA1 Transfer Complete */
    while(DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC1) == RESET);
    while(DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC2) == RESET);

    for(i = 0; i < RxBuffSize; i++)
    {
        printf("flt0:%d   flt1:%d\r\n", (RxdataFlt0[i] >> 8),(RxdataFlt1[i] >> 8));
    }	
    
    while(1);
}
