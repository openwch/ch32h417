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



/* I2C Mode Definition */
#define HOST_MODE  0
#define SLAVE_MODE 1

/* I2C Communication Mode Selection */
// #define I2C_MODE   HOST_MODE
#define I2C_MODE   SLAVE_MODE

/* Global define */
#define Size       6
#define Tize       6
#define RXAdderss  0x02
#define TxAdderss  0x02

/* Global Variable */
u8 TxData[Size] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
u8 RxData[5][Size];

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef  I2C_InitTSturcture = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC | RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_I2C2, ENABLE);

    // SCL PC0(AF9)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SDA PC1(AF9)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed          = bound;
    I2C_InitTSturcture.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle           = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1         = address;
    I2C_InitTSturcture.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitTSturcture);

    I2C_DMACmd(I2C2, ENABLE);

    I2C_Cmd(I2C2, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
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
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the SPI1 DMAy Channelx configuration.
 *
 * @param   DMA_CHx -
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
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
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{

    uint8_t i,j,t;

#if (I2C_MODE == HOST_MODE)
    printf("IIC Host mode\r\n");
    IIC_Init(80000, TxAdderss);

    for (j = 0; j < 5; j++)
    {
        DMA_Tx_Init(DMA1_Channel6, (u32)&I2C2->DATAR, (u32)TxData, Tize);
        DMA_MuxChannelConfig(DMA_MuxChannel6, 75);
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
            ;
        I2C_GenerateSTART(I2C2, ENABLE);

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
            ;
        I2C_Send7bitAddress(I2C2, 0x02, I2C_Direction_Transmitter);

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            ;

        DMA_Cmd(DMA1_Channel6, ENABLE);

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
        I2C_GenerateSTOP(I2C2, ENABLE);
        Delay_Ms(300);
    }

#elif (I2C_MODE == SLAVE_MODE)
    printf("IIC Slave mode\r\n");
    IIC_Init(80000, RXAdderss);

    for (t = 0; t < 5; t++)
    {
        DMA_Rx_Init(DMA1_Channel7, (u32)&I2C2->DATAR, (u32)&RxData[t][0], Tize);
        DMA_MuxChannelConfig(DMA_MuxChannel7, 76);
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED))
            ;
        DMA_Cmd(DMA1_Channel7, ENABLE);

        while ((!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC7)))
            ;

        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
            ;
        I2C2->CTLR1 &= I2C2->CTLR1;

        printf("RxData:\r\n");

        for (i = 0; i < 6; i++)
        {
            printf("%02x ", RxData[t][i]);
            RxData[t][i] = 0;
        }
        printf("\r\n ");
    }

#endif

    while (1)
        ;
}
