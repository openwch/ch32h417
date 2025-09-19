/********************************** (C) COPYRIGHT *******************************
 * File Name          : FMC_NOR.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/09/17
 * Description        : This file contains the headers of the FMC_NOR.
*********************************************************************************
* Copyright (c) 2024 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************/
#include <stdio.h>
#include "debug.h"
#include "string.h"
#include "FMC_NOR.h"
#include "math.h"
vu16 temptBuffer[256]={1,2,3,4};

/*********************************************************************
 * @fn      FMC_NorFlash_Init
 *
 * @brief   Init FMC
 *
 * @return  None
 */
void GPIO_Config(void)
{

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);


    // NOE PD4(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // NWE PD5(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // NWAIT PD6(AF12)
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

  // NE[1] PD7(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    // A[0] PF5(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // A[1] PB3(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[2] PB4(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[3] PB8(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[4] PB9(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[5] PC13(AF12)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // A[6] PA10(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // A[7] PA11(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // A[8] PA12(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // A[9] PA13(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // A[10] PB10(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[11] PB11(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[12] PB12(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[13] PB13(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[14] PB14(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[15] PB15(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // A[16] PD11(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // A[17] PD12(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // A[18] PD13(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // A[19] PE3(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // A[20] PE4(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // A[21] PE5(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // A[22] PE6(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[0] PD14(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[1] PD15(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[2] PD0(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[3] PD1(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[4] PE7(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[5] PE8(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[6] PE9(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[7] PE10(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[8] PE11(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[9] PE12(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[10] PE13(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[11] PE14(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[12] PE15(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[13] PD8(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[14] PD9(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[15] PD10(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

  

}
void FMC_NorFlash_Init(void)
{
    FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure={0};
    FMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC,ENABLE);
    GPIO_Config();

    readWriteTiming.FMC_AddressSetupTime = 0x08;
    readWriteTiming.FMC_AddressHoldTime = 0x00;
    readWriteTiming.FMC_DataSetupTime = 0x08;
    readWriteTiming.FMC_BusTurnAroundDuration = 0x08;
    readWriteTiming.FMC_CLKDivision = 0x00;
    readWriteTiming.FMC_DataLatency = 0x08;
    readWriteTiming.FMC_AccessMode = FMC_AccessMode_C;

    FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
    FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
    FMC_NORSRAMInitStructure.FMC_MemoryType =FMC_MemoryType_NOR;
    FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_16b;
    FMC_NORSRAMInitStructure.FMC_BurstAccessMode =FMC_BurstAccessMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_High;
    FMC_NORSRAMInitStructure.FMC_AsynchronousWait=FMC_AsynchronousWait_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
    FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
    FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
    FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Enable;
    FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
    FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &readWriteTiming;
    FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &readWriteTiming;

    FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);
    FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
}

/*********************************************************************
 * @fn      NOR_GetStatus
 *
 * @brief   Returns the NOR operation status
 *
 * @param   Timeout - NOR progamming Timeout
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_GetStatus(uint32_t Timeout)
{
    uint16_t val1 = 0x00, val2 = 0x00;
    NOR_Status status = NOR_ONGOING;
    uint32_t timeout = Timeout;

    /*!< Poll on NOR memory Ready/Busy signal ----------------------------------*/
    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) != RESET) && (timeout > 0))
    {
        timeout--;
    }
    timeout = Timeout;

    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET) && (timeout > 0))
    {
        timeout--;
    }

    /*!< Get the NOR memory operation status -----------------------------------*/
    while((Timeout != 0x00) && (status != NOR_SUCCESS))
    {
        Timeout--;

        /*!< Read DQ6 and DQ5 */
        val1 = *(__IO uint16_t *)(Bank1_NOR_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR_ADDR);

        /*!< If DQ6 did not toggle between the two reads then return NOR_Success */
        if((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }

        if((val1 & 0x0020) != 0x0020)
        {
            status = NOR_ONGOING;
        }

        val1 = *(__IO uint16_t *)(Bank1_NOR_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR_ADDR);

        if((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }
        else if((val1 & 0x0020) == 0x0020)
        {
            return NOR_ERROR;
        }
    }

    if(Timeout == 0x00)
    {
        status = NOR_TIMEOUT;
    }

    /*!< Return the operation status */
    return (status);
}

/*********************************************************************
 * @fn      NOR_Read_CFI
 *
 * @brief   Returns the NOR memory to Read  CFI mode
 *
 * @param   None
 *
 * @return  None
 */

void NOR_Read_CFI()
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x98);
}

/*********************************************************************
 * @fn      NOR_Read_ID_DMA
 *
 * @brief   Returns the NOR_ID by DMA
 *
 * @param   None
 *
 * @return  None
 */

void NOR_Read_ID_DMA(){

    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank1_NOR_ADDR+2*0);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)temptBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
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
    Delay_Ms(1000);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    printf("Manufacturer_Code=%x\r\n",temptBuffer[0x0]);
    printf("Device_Code1=%x\r\n",temptBuffer[0x1]);
    printf("Device_Code2=%x\r\n",temptBuffer[0xE]);
    printf("Device_Code3=%x\r\n",temptBuffer[0xF]);

}

/*********************************************************************
 * @fn      NOR_ReadID
 *
 * @brief   Reads NOR memory's Manufacturer and Device Code
 *
 * @return  None
 */

void NOR_ReadID()
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0090);
    NOR_Read_ID_DMA();

}

/*********************************************************************
 * @fn      NOR_ReturnToReadMode
 *
 * @brief   Returns the NOR memory to Read mode
 *
 * @param   none
 *
 * @return  NOR_SUCCESS
 */

NOR_Status NOR_ReturnToReadMode(void)
{
    NOR_WRITE(Bank1_NOR_ADDR, 0x00F0);

    return (NOR_SUCCESS);
}

/*********************************************************************
 * @fn      NOR_EraseBlock
 *
 * @brief   Erases the specified Nor memory block
 *
 * @param   BlockAddr: address of the block to erase
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

NOR_Status NOR_EraseBlock(uint32_t BlockAddr)
{
    u8 sta;
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR_ADDR + BlockAddr), 0x30);
    sta=NOR_GetStatus(BlockErase_Timeout);
    return (sta);
}

/*********************************************************************
 * @fn      NOR_EraseChip
 *
 * @brief   Erases the entire chip
 *
 * @param   None
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

NOR_Status NOR_EraseChip(void)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0010);

    return (NOR_GetStatus(ChipErase_Timeout));
}

/*********************************************************************
 * @fn      Fill_Buffer
 *
 * @brief   pBuffer: pointer on the Buffer to fill
 *          BufferSize: size of the buffer to fill
 *          Offset: first value to fill on the Buffer
 *
 * @param   None
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
    {
        pBuffer[IndexTmp] =IndexTmp +Offset;
    }
}

/*********************************************************************
 * @fn      NOR_WriteHalfWord
 *
 * @brief   Writes a half-word to the NOR memory
 *
 * @param   WriteAddr: NOR memory internal address to write to
 *          Data: Data to write
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00A0);
    NOR_WRITE((Bank1_NOR_ADDR + WriteAddr), Data);

    return (NOR_GetStatus(Program_Timeout));;
}

/*********************************************************************
 * @fn      NOR_WriteBuffer
 *
 * @brief   Writes a half-word buffer to the FMC NOR memory
 *
 * @param   pBuffer: pointer to buffer
 *          WriteAddr: NOR memory internal address from which the data will be
 *          written
 *          NumHalfwordToWrite: number of Half words to write
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
    NOR_Status status = NOR_ONGOING;
    do
    {
        /*!< Transfer data to the memory */
        status = NOR_WriteHalfWord(WriteAddr, *pBuffer++);
        WriteAddr = WriteAddr + 2;
        NumHalfwordToWrite--;

    }
    while((status == NOR_SUCCESS) && (NumHalfwordToWrite >0));
    return (status);
}

/*********************************************************************
 * @fn      NOR_ReadBuffer
 *
 * @brief   Reads a block of data from the FMC NOR memory
 *
 * @param   pBuffer: pointer to the buffer that receives the data read from the
 *          NOR memory
 *          ReadAddr: NOR memory internal address to read from
 *          NumHalfwordToRead : number of Half word to read
 * @return  None
 */



void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{

    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR_ADDR + ReadAddr), 0x00F0);
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank1_NOR_ADDR+ReadAddr);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = NumHalfwordToRead/16;
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
