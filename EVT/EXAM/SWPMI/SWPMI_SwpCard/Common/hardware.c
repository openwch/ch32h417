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
#define SWPMI_BITRATE                     (uint32_t)1000000

/* ACT-Frame POWER_MODE (Low power mode) */
/*
    Control field       0x62        Power mode indication
    Power mode          0x00        Low power mode
*/
uint32_t TxBuffer_ACT_POWER_MODE_LOW[1] = {0x00006202};

/* U-Frame RESET */
uint32_t TxBuffer_U_RESET[1] = {0x0004F903};

/* I-Frame OPEN_PIPE */
uint32_t TxBuffer_I_OPEN_PIPE[1] = {0x03818003};

/* 32 bit Data Buffer for reception*/ 
uint32_t RxBuffer[8] = {0};


/*********************************************************************
 * @fn      SWPMI_Function_Init
 *
 * @brief   Initializes SWPMI collection.
 *
 * @return  none
 */
void SWPMI_Function_Init(void)
{
    SWPMI_InitTypeDef SWPMI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SWPMI, ENABLE);
    SWPMI_InitStructure.BitRate = SWPMI_BITRATE;
    SWPMI_InitStructure.RxBufferingMode = DISABLE;
    SWPMI_InitStructure.TxBufferingMode = DISABLE;
    SWPMI_InitStructure.LoopBackMode = DISABLE;
    SWPMI_Init(&SWPMI_InitStructure);

    /* Enable the SWPMI transceiver */
    SWPMI_Cmd(ENABLE);
    /* Must wait for this flag to be set before setting the SWPACT bit to activate the SWP bus */
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RDY) == RESET);
    /* activate SWPMI */
    SWPMI_ActivateCmd(ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	u8 cnt = 0;
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOC, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF11);

    /* First must reset I/O state PC6 = 0 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    Delay_Ms(500);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    SWPMI_Function_Init();

    /* Wait for ACT_SYNC_ID */
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    /* Send ACT_POWER_MODE (Low power) command */
    SWPMI_TransmitData32(TxBuffer_ACT_POWER_MODE_LOW[0]);
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);
    /* Wait for end of activation from card */
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    /* Send U-Frame RESET command */
    SWPMI_TransmitData32(TxBuffer_U_RESET[0]);
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);
    /* Wait for the card response */
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    /* Send I-Frame OPEN_PIPE */
    SWPMI_TransmitData32(TxBuffer_I_OPEN_PIPE[0]);
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);
    /* Wait for the card response */
    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    printf("RxBuffer[0]:%08x\r\n",RxBuffer[0]);
    printf("RxBuffer[1]:%08x\r\n",RxBuffer[1]);
    printf("RxBuffer[2]:%08x\r\n",RxBuffer[2]);
    printf("RxBuffer[3]:%08x\r\n",RxBuffer[3]);
  
  while(1);

}
