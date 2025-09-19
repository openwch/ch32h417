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

#define SWPMI_BITRATE                     (uint32_t)1000000

/* Size of SWPMI frame (in word) */
#define SWPMI_FrameSize                  (uint32_t)8
/* Number of frames to be transferred */
#define SWPMI_TxFrameNum                 (uint32_t)9
/* Number of frames in the Multi buffer used by DMA */
#define SWPMI_MultiBufFrameNum           (uint32_t)6
/* Number of frames to copy in the multi buffer */
#define SWPMI_UpdateFrameNum             (SWPMI_TxFrameNum - SWPMI_MultiBufFrameNum)
/* Total size of Multi buffer (in word) */
#define SWPMI_MultibufSize               (uint32_t)(SWPMI_MultiBufFrameNum * SWPMI_FrameSize)
/* Total size of buffer used for reception (in word)*/
#define SWPMI_RxBufSize                  (uint32_t)(SWPMI_TxFrameNum * SWPMI_FrameSize)

/* Latest Frame length received at the end of the test */
#define SWPMI_LatestRFL                  (uint32_t)0x07

/* SWPMI_RxBufSize Data Buffer for multi buffer Reception */
uint32_t ReceiveBuff[SWPMI_RxBufSize];

/* SWPMI_MultibufSize Data Buffer for multi buffer mode Transmission (SWPMI_MultiBufFrameNum frames).
         (variable frame lengths = {0x1E, 0x03, 0x04, 0x1E, 0x0E, 0x15} as configured in the first data byte)*/ 
uint32_t MultiBuff[] =  
                               {0x1E11511E, 0x21115354, 0x21115556, 0x22225758, 0x3322595A, 0x44225B5C, 0x55225D5E, 0xFFFF5F60,
                                0x03030303, 0x00000000, 0x000FF000, 0x000FF000, 0x000000FF, 0xFF000000, 0x000000FF, 0x00000FF0,
                                0x04040404, 0x00000004, 0x000FF000, 0x000FF000, 0x000000FF, 0xFF000000, 0x000000FF, 0x00000FF0,
                                0x1E11511E, 0x21115354, 0x21115556, 0x22225758, 0x3322595A, 0x44225B5C, 0x55225D5E, 0xFFFF5F60,
                                0x1414140E, 0x14141414, 0x14141414, 0x00141414, 0x000000FF, 0x000000FF, 0xFF000000, 0xFF000000,
                                0x25252515, 0x25252525, 0x25252525, 0x25252525, 0x25252525, 0x25252525, 0x25252525, 0xFFFF2525};

/* Data Buffer used to transmit latest frames (SWPMI_UpdateFrameNum frames).
         (variable frame lengths = {0x05, 0x06, 0x07} as configured in the first data byte)*/ 
uint32_t LastFramesBuff[] =  
                               {0xCCBBAA05, 0x0000EEDD, 0x000FF000, 0x000FF000, 0x000000FF, 0xFF000000, 0x000000FF, 0x00000FF0,
                                0x06060606, 0x00060606, 0x000FF000, 0x000FF000, 0x000000FF, 0xFF000000, 0x000000FF, 0x00000FF0,
                                0x07070700 | SWPMI_LatestRFL, 0x07070707, 0x000FF000, 0x000FF000, 0x000000FF, 0xFF000000, 0x000000FF, 0x00000FF0};


__IO uint8_t  TxFramesCnt = 0;
__IO uint8_t  RxFramesCnt = 0;

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DFSDM filter 0 DMA Channelx configuration.
 *
 * @param   DMA_CHx - Select the DMA channel.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the DFSDM filter 0 DMA Channelx configuration.
 *
 * @param   DMA_CHx - Select the DMA channel.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize)
{
 DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);  
}


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
    SWPMI_InitStructure.RxBufferingMode = SWPMI_RxMode_Buffering_Multi;
    SWPMI_InitStructure.TxBufferingMode = SWPMI_TxMode_Buffering_Multi;
    SWPMI_InitStructure.LoopBackMode = ENABLE;
    SWPMI_Init(&SWPMI_InitStructure);

    SWPMI_TxBufferModeConfig(SWPMI_TxMode_Buffering_Multi);
    SWPMI_RxBufferModeConfig(SWPMI_RxMode_Buffering_Multi);

    SWPMI_Cmd(ENABLE);

    NVIC_SetPriority(SWPMI_IRQn, 0);   
    NVIC_EnableIRQ(SWPMI_IRQn);

    SWPMI_ITConfig(SWPMI_IT_TXUNR|SWPMI_IT_RXOVR|SWPMI_IT_RXBER, ENABLE);
    SWPMI_ITConfig(SWPMI_IT_RXBF|SWPMI_IT_TXBE,ENABLE);

    while(SWPMI_GetFlagStatus(SWPMI_FLAG_RDY) == RESET);
    SWPMI_ActivateCmd(ENABLE);
    
}

/*********************************************************************
 * @fn      WaitAndCheckEndOfTransfer
 *
 * @brief   Wait and check transfer data.
 *
 * @return  none
 */
void WaitAndCheckEndOfTransfer(void)
{
    /* Wait for end of transmission */
    while ((SWPMI->CR & SWPMI_TXDMA) == SET);

    /* Wait for end of reception */
    while (RxFramesCnt < SWPMI_TxFrameNum);
#ifdef Core_V5F
    __asm("fence");
#endif
    /* SWPMI DeInit */ 
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel1, DISABLE);
    SWPMI_ActivateCmd(DISABLE);

    /* Disable loopback mode */
    SWPMI_LoopbackCmd(DISABLE);
    /* Check that latest receive frame length */
    if (SWPMI_GetReceiveFrameLength() != SWPMI_LatestRFL)
    {
        printf("error..\r\n");
    }

    /* received data */
    for( int j = 0; j < SWPMI_RxBufSize; j++ )
    {
        printf("ReceiveBuff[%d] %08x\r\n",j,ReceiveBuff[j]);
    }        
}

/*********************************************************************
 * @fn      SWPMI_GPIO_INIT
 *
 * @brief   Initializes SWPMI gpio collection.
 *
 * @return  none
 */
void SWPMI_GPIO_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOC, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF11);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
    SWPMI_GPIO_INIT();

    SWPMI_Function_Init();

    DMA_Tx_Init(DMA1_Channel1,(uint32_t)&SWPMI->TDR,(uint32_t)MultiBuff,SWPMI_MultibufSize);
    DMA_Rx_Init(DMA1_Channel2,(uint32_t)&SWPMI->RDR,(uint32_t)ReceiveBuff,SWPMI_RxBufSize);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 101);/* swpmi tx */
    DMA_MuxChannelConfig(DMA_MuxChannel2, 102);/* swpmi rx */

    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    WaitAndCheckEndOfTransfer();

    while(1);
}

/**
  * @brief  This function handles SWPMI interrupt request.
  * @param  None
  * @retval None
  */
void SWPMI_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SWPMI_IRQHandler(void)
{
    /* SWPMI CRC error interrupt occurred */
    if(SWPMI_GetITStatus(SWPMI_IT_RXBER) != RESET)
    {
        SWPMI_ClearITPendingBit(SWPMI_IT_RXBER | SWPMI_IT_RXBF);
    }
    
    /* SWPMI Over-Run interrupt occurred */
    if(SWPMI_GetITStatus(SWPMI_IT_RXOVR) != RESET)
    {
        SWPMI_ClearITPendingBit(SWPMI_IT_RXOVR);
    }

    /* SWPMI Under-Run interrupt occurred */
    if(SWPMI_GetITStatus(SWPMI_IT_TXUNR) != RESET)
    {
        SWPMI_ClearITPendingBit(SWPMI_IT_TXUNR);
    }

    /* SWPMI in mode Transmitter (Transmit buffer empty) */
    if(SWPMI_GetITStatus(SWPMI_IT_TXBE) != RESET)
    {
        /* Clear the SWPMI Transmit buffer empty Flag */
        SWPMI_ClearITPendingBit(SWPMI_IT_TXBE);

        /* Increment frame number in transmission (or already transmitted) */
        TxFramesCnt++;

        /* Check if number of transmitted frames is not higher than Multi buffer size */
        if (TxFramesCnt == SWPMI_UpdateFrameNum)
        {
            memcpy(MultiBuff, LastFramesBuff, (SWPMI_UpdateFrameNum * 4 * SWPMI_FrameSize));
        }
        /* Check if reach number of frame to transmit*/
        else if (TxFramesCnt == SWPMI_TxFrameNum)
        {
            MultiBuff[(SWPMI_UpdateFrameNum + 1) * SWPMI_FrameSize] = 0;
        }
    }

  /* SWPMI in mode Receiver (Receive buffer full) */
    if((SWPMI_GetITStatus(SWPMI_IT_RXBF)!=RESET))
    {
        /* Clear the SWPMI Transmit buffer empty Flag */
        SWPMI_ClearITPendingBit(SWPMI_IT_RXBF);
        RxFramesCnt++;
    }
}