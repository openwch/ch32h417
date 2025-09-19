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

/* I2C Mode Definition */
#define HOST_MODE  0
#define SLAVE_MODE 1

/* I2C Communication Mode Selection */
// #define I2C_MODE   HOST_MODE
#define I2C_MODE   SLAVE_MODE

/* Global define */
#define Size       6
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

    I2C_Cmd(I2C2, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   main.
 *
 * @return  none
 */
void Hardware(void)
{
    u8 i = 0;
    u8 j = 0;
    u8 p = 0;
#if (I2C_MODE == HOST_MODE)
    printf("IIC Host mode\r\n");
    IIC_Init(80000, TxAdderss);

    for (j = 0; j < 5; j++)
    {
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
            ;

        I2C_GenerateSTART(I2C2, ENABLE);

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
            ;
        I2C_Send7bitAddress(I2C2, 0x02, I2C_Direction_Transmitter);

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            ;

        for (i = 0; i < 6; i++)
        {
            while (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET)
                ;
            I2C_SendData(I2C2, TxData[i]);
            Delay_Ms(20);
        }

        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
        I2C_GenerateSTOP(I2C2, ENABLE);
        Delay_Ms(300);
    }

#elif (I2C_MODE == SLAVE_MODE)
    printf("IIC Slave mode\r\n");
    IIC_Init(80000, RXAdderss);

    for (p = 0; p < 5; p++)
    {
        i = 0;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED))
            ;

        while (i < 6)
        {
            while (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
                ;
            RxData[p][i] = I2C_ReceiveData(I2C2);
            i++;
        }
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
            ;
        I2C2->CTLR1 &= I2C2->CTLR1;
    }

    printf("RxData:\r\n");
    for (p = 0; p < 5; p++)
    {
        for (i = 0; i < 6; i++)
        {
            printf("%02x ", RxData[p][i]);
        }
        printf("\r\n ");
    }

#endif

    while (1)
        ;
}
