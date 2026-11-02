/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "string.h"

/*
 *@Note
 SPI uses DMA, master/slave mode transceiver routine:
 Master:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1)\SPI2_NSS(PB12).
 Slave:SPI2_SCK(PB13)\SPI2_MISO(PC2)\SPI2_MOSI(PC1)\SPI2_NSS(PB12).

 This example demonstrates that Master and Slave use DAM full-duplex transmission
 and reception at the same time, repeated every 3s.
 The slave reception is interrupt-driven (DMA1_Channel2 TC interrupt): the ISR
 prints the received data, compares it with the transmitted data (same/diff),
 and restarts the SPI DMA for the next transfer.
 Note: The two boards download the Master and Slave programs respectively, and power
 on at the same time.
     Hardware connection:
               PB13 -- PB13
               PC2 -- PC2
               PC1 -- PC1
               PB12 -- PB12

*/

/* SPI Mode Definition */
#define HOST_MODE  0
#define SLAVE_MODE 1

/* SPI Communication Mode Selection */
// #define SPI_MODE   HOST_MODE
#define SPI_MODE   SLAVE_MODE

/* Global define */
#define Size       18

#define SPI_NSS_PORT   GPIOB
#define SPI_NSS_PIN    GPIO_Pin_12
#define SPI_NSS_LOW()  GPIO_ResetBits(SPI_NSS_PORT, SPI_NSS_PIN)
#define SPI_NSS_HIGH() GPIO_SetBits(SPI_NSS_PORT, SPI_NSS_PIN)

/* Global Variable */
u8 TxData[Size] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                   0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                   0x21, 0x22, 0x23, 0x24, 0x25, 0x26};
u8 RxData[Size];


void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI2, ENABLE);

#if (SPI_MODE == HOST_MODE)

    // SCK PB13(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MOSI PC1(AF5)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MISO PC2(AF5)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // NSS PB12(GPIO out)
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    SPI_NSS_HIGH();

#elif (SPI_MODE == SLAVE_MODE)

    // SCK PB13(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MOSI PC1(AF5)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MISO PC2(AF5)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // NSS PB12(GPIO in)
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

#if (SPI_MODE == HOST_MODE)
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

#elif (SPI_MODE == SLAVE_MODE)
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;

#endif

    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode6;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 8.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr    = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the SPI2 DMA Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 8.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr    = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_StartTransfer
 *
 * @brief   Starts a DMA transfer on the given channel without waiting.
 *
 * @param   DMAy_Channelx - DMA channel used for the transfer
 *          Data          - pointer to the data buffer
 *          DataLength    - number of data to transfer
 *
 * @return  none
 */
void DMA_StartTransfer(DMA_Channel_TypeDef* DMAy_Channelx, u8* Data, u16 DataLength)
{
    DMA_Cmd(DMAy_Channelx, DISABLE);
    /* Point to the new data buffer */
    DMAy_Channelx->MADDR = (uint32_t)Data;
    /* Set the transfer data count */
    DMAy_Channelx->CNTR = DataLength;
    /* Start the transfer */
    DMA_Cmd(DMAy_Channelx, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   SPI2 DMA full-duplex transfer, restarted every 3s.
 *
 * @return  none
 */
void Hardware(void)
{
    printf("SPI DMA TEST\r\n");

    SPI_FullDuplex_Init();

#if (SPI_MODE == HOST_MODE)
    printf("Host Mode\r\n");
#else
    printf("Slave Mode\r\n");
#endif

    DMA_Tx_Init(DMA1_Channel3, (u32)&SPI2->DATAR, (u32)TxData, Size);
    DMA_Rx_Init(DMA1_Channel2, (u32)&SPI2->DATAR, (u32)RxData, Size);

    DMA_MuxChannelConfig(DMA_MuxChannel3, 65);
    DMA_MuxChannelConfig(DMA_MuxChannel2, 66);

#if (SPI_MODE == SLAVE_MODE)
    /* Enable the SPI2 RX DMA (CH2) transfer-complete interrupt */
    DMA_ClearITPendingBit(DMA1, DMA1_IT_TC2);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
#endif

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);

#if (SPI_MODE == HOST_MODE)
    SPI_NSS_LOW();
    Delay_Ms(1);
#endif

    DMA_StartTransfer(DMA1_Channel2, RxData, Size);
    DMA_StartTransfer(DMA1_Channel3, TxData, Size);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

#if (SPI_MODE == HOST_MODE)
    while ((!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC2)) || (!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC3)))
        ;
    Delay_Ms(1);
    SPI_NSS_HIGH();
#endif


    while (1)
    {
#if (SPI_MODE == HOST_MODE)
        Delay_Ms(3000);

        SPI_NSS_LOW();
        Delay_Ms(1);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);
        /* Start a new full-duplex transfer */
        DMA_StartTransfer(DMA1_Channel2, RxData, Size);
        DMA_StartTransfer(DMA1_Channel3, TxData, Size);

        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

        while ((!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC2)) || (!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC3)))
            ;

        Delay_Ms(1);
        SPI_NSS_HIGH();

        DMA_ClearFlag(DMA1, DMA1_FLAG_TC2);
        DMA_ClearFlag(DMA1, DMA1_FLAG_TC3);

        printf("tx\n");
#else

#endif
    }
}

/*********************************************************************
 * @fn      DMA1_Channel2_IRQHandler
 *
 * @brief   This function handles DMA1 Channel 2 (SPI2 RX) global interrupt.
 *
 * @return  none
 */
void DMA1_Channel2_IRQHandler(void)
{
    u8 i = 0;

    if (DMA_GetITStatus(DMA1, DMA1_IT_TC2) != RESET)
    {
        DMA_ClearITPendingBit(DMA1, DMA1_IT_TC2);

        for (i = 0; i < Size; i++)
        {
            printf(" RxData:%02x\r\n", RxData[i]);
        }

        if (memcmp(TxData, RxData, sizeof(TxData)) == 0)
        {
            printf("same\r\n");
        }
        else
        {
            printf("diff\r\n");
        }

        /* Restart the SPI2 DMA for the next transfer */
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);
        DMA_StartTransfer(DMA1_Channel2, RxData, Size);
        DMA_StartTransfer(DMA1_Channel3, TxData, Size);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
    }
}
