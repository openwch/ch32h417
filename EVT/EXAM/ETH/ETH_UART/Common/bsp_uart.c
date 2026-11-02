/********************************** (C) COPYRIGHT *******************************
* File Name          : bsp_uart.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/09/18
* Description        : uart cfg.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "bsp_uart.h"

/* Global Variable */

struct uart_data  uart_data_t;
/* UART2 */
u8  UART2_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
u8  UART2_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
u16 UART2_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/**********************************************************************
 *  @fn      UART_GPIO_Init
 *
 *  @brief   initialize uart2 GPIO. USART2 TX-->PD5  RX-->PD6 (AF7).
 *
 *  @return  none
 * */
void UART_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);

    /* UART2 TX-->PD5   RX-->PD6 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/**********************************************************************
 *  @fn      UART_DMA_Init
 *
 *  @brief   initialize uart2 DMA. Mux 87/88 = USART2_TX/RX.
 *
 *  @return  none
 * */
void UART_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    /* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)uart_data_t.TX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    /* The TX DMA transfer length is deliberately not configured here: uartTx()
     * points MADDR at the buffer that is to be sent and writes CNTR right
     * before enabling the channel, so the length is set per transfer. */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)uart_data_t.RX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_MuxChannelConfig(DMA_MuxChannel7, 87); /* USART2_TX */
    DMA_MuxChannelConfig(DMA_MuxChannel6, 88); /* USART2_RX */

    DMA_ClearFlag(DMA1, DMA1_IT_TC7);

    DMA_Cmd(DMA1_Channel6, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
}

/**********************************************************************
 *  @fn      UART_Interrupt_Init
 *
 *  @brief   initialize uart2 interrupt
 *
 *  @return  none
 * */
void UART_Interrupt_Init(void)
{
    /* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
    DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/**********************************************************************
 *  @fn      BSP_Uart_Init
 *
 *  @brief   initialize uart2
 *
 *  @return  none
 * */
void BSP_Uart_Init(uint32_t baudrate )
{
    USART_InitTypeDef USART_InitStructure = {0};

    UART_GPIO_Init();

    /* USART2 TX-->PD5   RX-->PD6 */
    uart_data_t.RX_buffer = UART2_RX_DMA_DataBuf;
    uart_data_t.TX_buffer = &UART2_TX_DMA_DataBuf[0];
    uart_data_t.TX_data_length = UART2_TX_Buf_Length;
    uart_data_t.rx_read = 0;
    uart_data_t.rx_write = 0;
    uart_data_t.last_RX_DMA_length = UART_RX_DMA_SIZE;
    uart_data_t.RX_data_length = 0;
    uart_data_t.tx_read = 0;
    uart_data_t.tx_write = 0;
    uart_data_t.tx_remainBuffNum = UART_TX_BUF_NUM;
    uart_data_t.uart_tx_dma_state = IDLE;

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_USART2, ENABLE);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

    UART_DMA_Init();
    UART_Interrupt_Init();
}
