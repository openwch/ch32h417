/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/10/20
* Description        : This file provides some functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "math.h"

#define PI          3.1415926535f
#define TAU         (2.0f * PI)
#define SAMPLE_RATE (8000)
#define DATA_SIZE   SAMPLE_RATE

s16 SAI_Data[DATA_SIZE];

/*********************************************************************
 * @fn      GPIO_Config
 *
 * @brief   Configure the GPIO.
 *
 * @return  none
 */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
	
    // FS_A PC3(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SCK_A PC2(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SD_A PC1(AF6)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MCLK_A PC0(AF7)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
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
    SAI_InitStructure.SAI_FIFOThreshold = SAI_Threshold_FIFOEmpty;

    SAI_Init(SAI_Block_A, &SAI_InitStructure);

    SAI_FrameInitStructure.SAI_FrameLength       = 32;
    SAI_FrameInitStructure.SAI_ActiveFrameLength = 16;
    SAI_FrameInitStructure.SAI_FSDefinition      = SAI_FS_StartFrame;
    SAI_FrameInitStructure.SAI_FSPolarity        = SAI_FS_ActiveHigh;
    SAI_FrameInitStructure.SAI_FSOffset          = SAI_FS_BeforeFirstBit;
    SAI_FrameInit(SAI_Block_A, &SAI_FrameInitStructure);

    SAI_SlotInitStructure.SAI_FirstBitOffset = 0;
    SAI_SlotInitStructure.SAI_SlotSize       = SAI_SlotSize_16b;
    SAI_SlotInitStructure.SAI_SlotNumber     = 2;
    SAI_SlotInitStructure.SAI_SlotActive     = SAI_SlotActive_ALL;
    SAI_SlotInit(SAI_Block_A, &SAI_SlotInitStructure);

    SAI_FlushFIFO(SAI_Block_A);
}

/*********************************************************************
 * @fn      Tx_DMA_Init
 *
 * @brief   Initialize the DMA for transmitting data.
 *
 * @param   DMAy_Channelx - the DMA channel to be initialized.
 *           pAddr - the address of the peripheral.
 *           mAddr - the address of the memory.
 *           Length - the length of the data to be transmitted.
 *
 * @return  none
 */
void Tx_DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t pAddr, uint32_t mAddr,
                 uint16_t Length)
{

    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMAy_Channelx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = pAddr;
    DMA_InitStructure.DMA_Memory0BaseAddr    = mAddr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = Length;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMAy_Channelx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   main program.
 *
 * @return  none
 */
void Hardware(void)
{
    printf("SAI TEST\r\n");

    // Generate the sine wave data
    // The sine wave frequency is 440Hz(A4) and the sampling rate is 8000Hz.
    for (int i = 0; i < DATA_SIZE; i += 2)
    {
        float t         = ((float)i) / ((float)SAMPLE_RATE);
        SAI_Data[i]     = 1000.0f * sinf(440.0f * TAU * t);
        SAI_Data[i + 1] = SAI_Data[i];
    }

    SAI_Config(SAMPLE_RATE);
    GPIO_Config();

    Tx_DMA_Init(DMA1_Channel1, (uint32_t)(&SAI_Block_A->DATAR), (uint32_t)SAI_Data, DATA_SIZE);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 112);

    DMA_Cmd(DMA1_Channel1, ENABLE);
    SAI_DMACmd(SAI_Block_A, ENABLE);
    
    SAI_Cmd(SAI_Block_A, ENABLE);

    while (1)
    {
    }
}
