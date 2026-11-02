/*
 * app_uart.c
 *
 *  Created on: Sep 9, 2022
 *      Author: TECH66
 */
#include "app_uart.h"
#include "app_iochub.h"

extern uart_device_t s_uart_dev[COMNUM];

void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      uart3_init
 *
 * @brief   Initializes the USART3 peripheral.
 *
 * @return  none
 */
void uart3_init (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB1PeriphClockCmd (RCC_HB1Periph_USART3, ENABLE);
    RCC_HB2PeriphClockCmd (RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HBPeriphClockCmd (RCC_HBPeriph_DMA1, ENABLE);

    /* USART3 TX-->B.10  RX-->B.11 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF7);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART3, &USART_InitStructure);
    USART_ITConfig (USART3, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    
    USART_DMACmd (USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_EnableIRQ(USART3_IRQn);

    NVIC_EnableIRQ(DMA1_Channel2_IRQn);/* UART3 DMA1Tx*/

    NVIC_EnableIRQ(DMA1_Channel3_IRQn);/* UART3 DMA1Rx*/
    USART_Cmd (USART3, ENABLE);
}

/**
 * @brief  uart3 dma send channel configuration
 * @param  mem_addr - pointer to the data buffer
 * @param  mem_size - number of bytes to transfer
 * @retval none
 */
void uart3_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel2);
    DMA_Cmd (DMA1_Channel2, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel2, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel2, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA1,DMA1_IT_TC2); /* Clearing the delivery completion flag */
    DMA_Cmd (DMA1_Channel2, DISABLE);

    DMA_MuxChannelConfig(DMA_MuxChannel2, 89);
}
/**
 * @brief  uart3 dma send enable
 * @param  mem_addr - pointer to the data buffer
 * @param  mem_size - number of bytes to transfer
 * @retval none
 */
void uart3_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA1_Channel2->CFGR &= ~0x00000001;
    DMA1_Channel2->CNTR = mem_size;
    DMA1_Channel2->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA1,DMA1_FLAG_TC2);
    DMA1_Channel2->CFGR |= 0x00000001;
}

/**
 * @brief  uart3 dma receive channel configuration
 * @param  mem_addr - pointer to the data buffer
 * @param  mem_size - number of bytes to transfer
 * @retval none
 */
void uart3_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel3);
    DMA_Cmd (DMA1_Channel3, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel3, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel3, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA1,DMA1_IT_TC3);
    DMA_ClearFlag (DMA1,DMA1_IT_HT3);
    DMA_Cmd (DMA1_Channel3, ENABLE);

    DMA_MuxChannelConfig(DMA_MuxChannel3, 90);
}

uint16_t uart_get_dmarx_buf_remain_size (uint8_t uart_id) {
    uint16_t counter = 0;
    switch (uart_id) {
    case DEV_UART3:
#if UART3_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA1_Channel3);
#endif
        break;

    default:
        break;
    }
    return counter;
}

/**
 * @brief  UART DMA Rx completion ISR
 * @param  uart_id - UART device index (DEV_UARTx)
 * @retval none
 */
void uart_dmarx_done_isr (uint8_t uart_id) {
    uint16_t recv_size;

    recv_size = s_uart_dev[uart_id].dmarx_buf_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, NULL, 0);

    s_uart_dev[uart_id].last_dmarx_size = 0;
}

/**
 * @brief  UART DMA HT (Half-Transfer) ISR
 * @param  uart_id - UART device index (DEV_UARTx)
 * @retval none
 */
void uart_dmarx_half_done_isr (uint8_t uart_id) {
    uint16_t recv_total_size;
    uint16_t recv_size;
    lwrb_sz_t wsize = 0;

    recv_total_size = s_uart_dev[uart_id].dmarx_buf_size - uart_get_dmarx_buf_remain_size (DEV_UART3);

    recv_size = recv_total_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, &wsize, 0);
    s_uart_dev[uart_id].last_dmarx_size = recv_total_size;
}

/**
 * @brief  UART IDLE ISR
 * @param  uart_id - UART device index (DEV_UARTx)
 * @retval none
 */
void uart_dmarx_idle_isr (uint8_t uart_id) {
    uint16_t recv_total_size;
    uint16_t recv_size;
    lwrb_sz_t wsize = 0;

    recv_total_size = s_uart_dev[uart_id].dmarx_buf_size - uart_get_dmarx_buf_remain_size (DEV_UART3);

    recv_size = recv_total_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, &wsize, 0);

    s_uart_dev[uart_id].last_dmarx_size = recv_total_size;
}

/**
 * @brief  UART DMA TC (Transfer Complete) ISR
 * @param  uart_id - UART device index (DEV_UARTx)
 * @retval none
 */
void uart_dmatx_done_isr (uint8_t uart_id) {
    s_uart_dev[uart_id].status = 0;
}



/*********************************************************************
 * @fn      DMA1_Channel2_IRQHandler
 *
 * @brief   uart3 tx
 *
 * @return  none
 */
void DMA1_Channel2_IRQHandler (void) {
    if (DMA_GetITStatus (DMA1,DMA1_IT_TE2)) {
        DMA_ClearFlag (DMA1,DMA1_IT_TE2);
    }

    if (DMA_GetITStatus (DMA1,DMA1_IT_TC2)) {
    //   printf("tc\r\n");
        uart_dmatx_done_isr (DEV_UART3);
        DMA_ClearFlag (DMA1,DMA1_FLAG_TC2);
        DMA_Cmd (DMA1_Channel2, DISABLE);
    }
}
/*********************************************************************
 * @fn      DMA1_Channel3_IRQHandler
 *
 * @brief   uart3 rx
 *
 * @return  none
 */
void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1,DMA1_IT_TE3))
	{
		DMA_ClearFlag(DMA1,DMA1_IT_TE3);
	}
	
    if(DMA_GetITStatus(DMA1,DMA1_IT_HT3))
	{
		DMA_ClearFlag(DMA1,DMA1_FLAG_HT3);
		uart_dmarx_half_done_isr(DEV_UART3);
	}
	if(DMA_GetITStatus(DMA1,DMA1_IT_TC3))
	{
		DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
		uart_dmarx_done_isr(DEV_UART3);
	}
}
/*********************************************************************
 * @fn      USART3_IRQHandler
 *
 * @brief   uart3 IRQHandler
 *
 * @return  none
 */
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		uart_dmarx_idle_isr(DEV_UART3);
		USART_ReceiveData(USART3);//Clear the idle flag
	}
}