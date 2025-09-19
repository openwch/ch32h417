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



/* Global Variable */
#define _16bit__data_mode 0  //<= 16bit
#define _32bit__data_mode 1  //> 16bit

#define data_len_mode     _16bit__data_mode
// #define  data_len_mode   _32bit__data_mode

#define Len               10

u32 I2S3_Tx[Len];
u32 I2S2_Rx[Len];

/*********************************************************************
 * @fn      I2S2_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef  I2S_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI2, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);

    // WS PB12(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CK PB13(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD PC1(AF5)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    I2S_InitStructure.I2S_Mode     = I2S_Mode_MasterRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if (data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq  = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL       = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);
}

/*********************************************************************
 * @fn      I2S3_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef  I2S_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI3, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

	// WS PA15(AF6)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF6);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// CK PA14(AF1)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF1);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// SD PA13(AF1)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF1);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode     = I2S_Mode_SlaveTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if (data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq  = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL       = I2S_CPOL_High;
    I2S_Init(SPI3, &I2S_InitStructure);
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
    uint32_t i;
    for (i = 0; i < Len; i++)
    {
#if (data_len_mode == _16bit__data_mode)
        I2S3_Tx[i] = 0x5aa1 + i;

#elif (data_len_mode == _32bit__data_mode)
        I2S3_Tx[i] = 0x1AA15aa1 + i + (i << 28);

#endif
    }

    I2S3_Init();
    I2S2_Init();

#if (data_len_mode == _16bit__data_mode)
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
        ;
    SPI_I2S_SendData(SPI3, I2S3_Tx[0]);
#elif (data_len_mode == _32bit__data_mode)
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
        ;
    SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[0] >> 16));

#endif

    I2S_Cmd(SPI3, ENABLE);
    I2S_Cmd(SPI2, ENABLE);

#if (data_len_mode == _16bit__data_mode)
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
        ;
    SPI_I2S_SendData(SPI3, I2S3_Tx[1]);

#elif (data_len_mode == _32bit__data_mode)
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
        ;
    SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[0]));

#endif

    for (i = 0; i < Len; i++)
    {
#if (data_len_mode == _16bit__data_mode)
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1)
            ;
        I2S2_Rx[i] = SPI_I2S_ReceiveData(SPI2);

        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
            ;
        SPI_I2S_SendData(SPI3, (u16)I2S3_Tx[i + 2]);

#elif (data_len_mode == _32bit__data_mode)

        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1)
            ;
        I2S2_Rx[i] = SPI_I2S_ReceiveData(SPI2);
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
            ;
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[i + 1] >> 16));
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1)
            ;
        I2S2_Rx[i] = (u32)(SPI_I2S_ReceiveData(SPI2)) + (u32)(I2S2_Rx[i] << 16);
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1)
            ;
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[i + 1]));

#endif
    }

    I2S_Cmd(SPI3, DISABLE);
    I2S_Cmd(SPI2, DISABLE);

    printf("Rx data:\r\n");
    for (i = 0; i < Len; i++)
    {

        printf("%08x %08x\r\n", I2S3_Tx[i], I2S2_Rx[i]);
    }

    while (1)
        ;
    while (1)
        ;
}
