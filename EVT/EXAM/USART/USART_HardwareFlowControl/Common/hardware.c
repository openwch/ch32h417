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




/* Global typedef */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

/* Global define */
#define TxSize     (size(TxBuffer))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global Variable */
u8 TxBuffer[] = "USART1 Hardware Flow Control Example: USART1-\
Hyperterminal communication using hardware flow control\n\r";
u8 RxBuffer[TxSize] = {0};
u8 TxCnt = 0, RxCnt = 0;

/*********************************************************************
 * @fn      Buffercmp
 *
 * @brief   Compares two buffers
 *
 * @param   Buf1,Buf2 - buffers to be compared
 *          BufferLength - buffer's length
 *
 * @return  PASSED - Buf1 identical to Buf
 *          FAILED - Buf1 differs from Buf2
 */
TestStatus Buffercmp(uint8_t *Buf1, uint8_t *Buf2, uint16_t BufLength)
{
    while(BufLength--)
    {
        if(*Buf1 != *Buf2)
        {
            return FAILED;
        }
        Buf1++;
        Buf2++;
    }
    return PASSED;
}

/*********************************************************************
 * @fn      USART1_ReCFG
 *
 * @brief   Reinitialize the USART1.
 *
 * @return  none
 */
void USART1_ReCFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_USART1, ENABLE);

    /* USART1 TX-->A.9   RX-->A.10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; /* RTS-->A.12 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; /* CTS-->A.11 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
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
    USART1_ReCFG();	;
    printf("USART1 Hardware Flow Control TEST\r\n");
    while(TxCnt < TxSize)
    {
        USART_SendData(USART1, TxBuffer[TxCnt++]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            ;
    }
    do
    {
        if((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) && (RxCnt < TxSize))
        {
            RxBuffer[RxCnt] = USART_ReceiveData(USART1);
            USART_SendData(USART1, RxBuffer[RxCnt++]); /* send it when receive a byte */
        }
    } while((RxBuffer[RxCnt - 1] != '\r') && (RxCnt != TxSize));


	while (1)
		;
}
