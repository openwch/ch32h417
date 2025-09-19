/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
/* Global define */
#define Bank1_SRAM1_ADDR       ((u32)(0x60000000))
#define WRITE_READ_ADDR        ((uint32_t)0x0)
#define BUFFER_SIZE            1024*5
/* Global Variable */
uint32_t TxBuffer[BUFFER_SIZE]={1,11,2,22,3,33,4,77};
uint32_t RxBuffer[BUFFER_SIZE]={1,11,2,22,3,33,4,77};
volatile uint32_t WriteReadStatus = 0, Index = 0;
/*********************************************************************
 * @fn      Fill_Buffer
 *
 * @brief   pBuffer: pointer on the Buffer to fill
 *          BufferSize: size of the buffer to fill
 *          Offset: first value to fill on the Buffer
 *
 * @param   None
 *
 * @return  None
 *          
 */

void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
    {
        pBuffer[IndexTmp] =IndexTmp +Offset;
    }
}
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);

    // NE[1] PD7(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // NADV PB7(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // NBL[0] PC12(AF0)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // NBL[1] PC11(AF0)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // NOE PD4(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // NWE PD5(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // A[16] PD11(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // A[17] PD12(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[0] PD14(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[1] PD15(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[2] PD0(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[3] PD1(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[4] PE7(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[5] PE8(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[6] PE9(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[7] PE10(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[8] PE11(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[9] PE12(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[10] PE13(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[11] PE14(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[12] PE15(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[13] PD8(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[14] PD9(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[15] PD10(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

}


/*********************************************************************
 * @fn      FMC_SRAM_Init
 *
 * @brief   Init FMC
 *
 * @return  none
 */
void FMC_SRAM_Init(void)
{
    FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure={0};
    FMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC, ENABLE);
    GPIO_Config();


    readWriteTiming.FMC_AddressSetupTime = 0x01;
    readWriteTiming.FMC_AddressHoldTime = 0x00;
    readWriteTiming.FMC_DataSetupTime = 0x03;
    readWriteTiming.FMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FMC_AccessMode = FMC_AccessMode_A;

    FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
    FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Enable;
    FMC_NORSRAMInitStructure.FMC_MemoryType =FMC_MemoryType_SRAM;
    FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_16b;
    FMC_NORSRAMInitStructure.FMC_BurstAccessMode =FMC_BurstAccessMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
    FMC_NORSRAMInitStructure.FMC_AsynchronousWait=FMC_AsynchronousWait_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
    FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
    FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
    FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
    FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &readWriteTiming;
    FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &readWriteTiming;

    FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);
    FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
}
/*********************************************************************
 * @fn      FSMC_SRAM_WriteBuffer
 *
 * @brief   Write data to NOR_SRAM
 *
 * @param   pBuffer - data pointer
 *          WriteAddr - Start address
 *          n - data number
 *
 * @return  none
 */
void FMC_SRAM_WriteBuffer(u32* pBuffer,u32 WriteAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *(vu32*)(Bank1_SRAM1_ADDR+WriteAddr)=*pBuffer;
        WriteAddr=WriteAddr+4;
        pBuffer++;
    }
}
/*********************************************************************
 * @fn      FMC_SRAM_WriteBuffer_DMA
 *
 * @brief   Write buffer by DMA.
 *
 * @return  none
 */

void FMC_SRAM_WriteBuffer_DMA(u32 *pBuffer,u32 WriteAddr,u32 n)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank1_SRAM1_ADDR+WriteAddr);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = n/8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_256;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_256;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
	while(DMA_GetFlagStatus(DMA1,DMA1_FLAG_TC3) == RESET)
    {
    }
}
/*********************************************************************
 * @fn      FMC_SRAM_ReadBuffer_DMA
 *
 * @brief   Read buffer by DMA.
 *
 * @return  none
 */

void FMC_SRAM_ReadBuffer_DMA(u32 *pBuffer,u32 ReadAddr,u32 n)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_Cmd(DMA1_Channel3, DISABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank1_SRAM1_ADDR+ReadAddr);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = n/8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_256;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_256;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
	while(DMA_GetFlagStatus(DMA1,DMA1_FLAG_TC3) == RESET)
    {
        
    }
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
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
    FMC_SRAM_Init();
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x01);
    // FMC_SRAM_WriteBuffer(TxBuffer,WRITE_READ_ADDR,BUFFER_SIZE);
    FMC_SRAM_WriteBuffer_DMA(TxBuffer,WRITE_READ_ADDR,BUFFER_SIZE);
    FMC_SRAM_ReadBuffer_DMA(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    for ( Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
    {
        if (RxBuffer[Index] != TxBuffer[Index])
        {
            printf("error\n");
             WriteReadStatus = Index + 1;
            printf("RxBuffer[%d]=%x\r\n",Index,RxBuffer[Index]);
            printf("TxBuffer[%d]=%x\r\n",Index,TxBuffer[Index]);
        }
    }
  
        if (WriteReadStatus == 0)
    {
        /* OK */
        printf("sucess\r\n");
    }
    else
    {
        /* KO */
        printf("error\r\n");
    }
    printf("end\n");
    while(1);
}
