/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_compatibility_hid.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/07/16
* Description        : 
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32h417_usbfs_device.h"
#include "usbd_compatibility_hid.h"


/******************************************************************************/
/* Macro definitions */
#define DEF_UART                   USART4

#define DEF_UART_TX_MUX_CH         DMA_MuxChannel1
#define DEF_UART_TX_DMA_CH         DMA1_Channel1       
#define DEF_UART_TX_DMA_REQ        91

#define DEF_UART_RX_MUX_CH         DMA_MuxChannel2   
#define DEF_UART_RX_DMA_CH         DMA1_Channel2
#define DEF_UART_RX_DMA_REQ        92


void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

__attribute__ ((aligned(4))) uint8_t UART_RxBuffer[DEF_UART_BUF_SIZE];  // UART2 Rx Buffer
__attribute__ ((aligned(4))) uint8_t  HID_Report_Buffer[64];              // HID Report Buffer
volatile uint8_t HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;               // HID SetReport flag

volatile uint16_t UART_TimeOut;                                                 // UART RX timeout flag
volatile uint8_t  UART_Tx_Flag = 0;                                             // UART TX flag

volatile uint16_t UART_RX_CurCnt = 0;                                           // UART DMA current remain count
volatile uint16_t UART_RX_LastCnt = 0;                                          // UART DMA last remain count
volatile uint16_t UART_Rx_RemainLen = 0;                                        // UART RX data remain len
volatile uint16_t UART_Rx_Deal_Ptr = 0;                                         // UART RX data deal pointer

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Timer2 100us initialisation
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );

    NVIC_SetPriority(TIM2_IRQn,0x80);                                           // Preemption Priority:1
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   TIM2 IRQ handler
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    UART_TimeOut++;
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

/*********************************************************************
 * @fn      UART_DMA_Init
 *
 * @brief   UART DMA initialization
 *
 * @return  none
 */
void UART_DMA_Init( void )
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

    /* UART Tx DMA initialization */
    DMA_Cmd( DEF_UART_TX_DMA_CH, DISABLE );
    DMA_DeInit( DEF_UART_TX_DMA_CH );
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&DEF_UART->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Data_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DEF_USBD_HS_PACK_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DEF_UART_TX_DMA_CH, &DMA_InitStructure );
    DMA_MuxChannelConfig(DEF_UART_TX_MUX_CH, DEF_UART_TX_DMA_REQ);

    /* UART Rx DMA initialization */
    DMA_Cmd( DEF_UART_RX_DMA_CH, DISABLE );
    DMA_DeInit( DEF_UART_RX_DMA_CH );
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART_RxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DEF_UART_BUF_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init( DEF_UART_RX_DMA_CH, &DMA_InitStructure );
    DMA_MuxChannelConfig(DEF_UART_RX_MUX_CH, DEF_UART_RX_DMA_REQ);
    DMA_Cmd( DEF_UART_RX_DMA_CH, ENABLE );
    USART_DMACmd(DEF_UART, USART_DMAReq_Rx, ENABLE);
}


/*********************************************************************
 * @fn      UART_Init
 *
 * @brief   UART DMA initialization
 *
 * @return  none
 */
void UART_Init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOF, ENABLE );
    RCC_HB1PeriphClockCmd( RCC_HB1Periph_USART4, ENABLE );

    /* UART GPIO Init */
    GPIO_SetBits( GPIOF, GPIO_Pin_4 );
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOF, &GPIO_InitStructure );

    /* TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOF, &GPIO_InitStructure );
    GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF7);
    /* RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOF, &GPIO_InitStructure );
    GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF7);

    /* UART Init */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(DEF_UART, &USART_InitStructure);

    USART_ClearFlag( DEF_UART, USART_FLAG_TC );
    USART_Cmd(DEF_UART, ENABLE);
}

void UART_DMA_Tx(uint8_t *pbuf,uint16_t len)
{
    USART_ClearFlag(DEF_UART, USART_FLAG_TC);
    DMA_Cmd( DEF_UART_TX_DMA_CH, DISABLE );
    DEF_UART_TX_DMA_CH->MADDR = (uint32_t)pbuf;
    DEF_UART_TX_DMA_CH->CNTR = (uint32_t)len;
    DMA_Cmd( DEF_UART_TX_DMA_CH, ENABLE );
    USART_DMACmd(DEF_UART, USART_DMAReq_Tx, ENABLE);
}

/*********************************************************************
 * @fn      UART2_Rx_Service
 *
 * @brief   UART2 rx service routine that sends the data received by
 *          uart2 via USB-HID.
 *
 * @return  none
 */
void UART_Rx_Service( void )
{
    uint16_t pkg_len;
    uint16_t u16_temp;
    uint16_t remain_len;

    /* Get uart rx data */
    UART_RX_CurCnt = DMA_GetCurrDataCounter(DEF_UART_RX_DMA_CH);                              // Get DMA remaining count
    if (UART_RX_LastCnt != UART_RX_CurCnt)
    {
        if (UART_RX_LastCnt > UART_RX_CurCnt)
        {
            u16_temp = UART_RX_LastCnt - UART_RX_CurCnt;
        }
        else
        {
            u16_temp = UART_RX_LastCnt + ( DEF_UART_BUF_SIZE - UART_RX_CurCnt );
        }

        UART_RX_LastCnt = UART_RX_CurCnt;

        if ((UART_Rx_RemainLen + u16_temp) > DEF_UART_BUF_SIZE )
        {
            printf("Uart RX_buffer overflow\n");                                           // overflow: New data overwrites old data
        }
        else
        {
            UART_Rx_RemainLen += u16_temp;
        }
        UART_TimeOut = 0;
    }

    /* Pass-through uart data to USB-HID  */
    if (UART_Rx_RemainLen)
    {
        /* Determine if the USB endpoint is allowed to upload */
        if ((USBFSD->UEP2_TX_CTRL & USBFS_UEP_T_RES_MASK) == USBFS_UEP_T_RES_NAK )
        {
            /* Calculate the length of this packet upload via USB. */
            remain_len = UART_Rx_RemainLen;
            pkg_len = 0x00;
            if (remain_len >= ( DEF_USBD_FS_PACK_SIZE - 1 ))
            {
                remain_len = ( DEF_USBD_FS_PACK_SIZE - 1 );
                pkg_len = remain_len;
            }
            else
            {
                if (UART_TimeOut > DEF_UART_TOUT_TIME)
                {
                    pkg_len = remain_len;
                }
            }

            if ( pkg_len > ( DEF_UART_BUF_SIZE - UART_Rx_Deal_Ptr )  )
            {
                pkg_len = ( DEF_UART_BUF_SIZE - UART_Rx_Deal_Ptr );                       // The data at the end of the buffer will be sent directly.
            }

            /* Upload packet via USB. */
            if (pkg_len)
            {
                USBFSD->UEP2_DMA = (uint32_t)USBFS_EP2_Buf;
                USBFS_EP2_Buf[0] = pkg_len;
                memcpy(USBFS_EP2_Buf + 1,&UART_RxBuffer[UART_Rx_Deal_Ptr],pkg_len);                
                USBFSD->UEP2_TX_LEN = 64;
                USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;            // Start Upload
                UART_Rx_RemainLen -= pkg_len;
                UART_Rx_Deal_Ptr += pkg_len;
                if (UART_Rx_Deal_Ptr >= DEF_UART_BUF_SIZE)
                {
                    UART_Rx_Deal_Ptr = 0x00;
                }

            }
        }
    }

}

/*********************************************************************
 * @fn      UART_Tx_Service
 *
 * @brief   UART tx service routine that sends the data received by
 *          USB-HID via uart.
 *
 * @return  none
 */
void UART_Tx_Service( void )
{
    uint16_t pkg_len = 0;
    uint8_t *pbuf;
    if (UART_Tx_Flag)
    {
        if (USART_GetFlagStatus(DEF_UART, USART_FLAG_TC))                                  // Check whether uart has finished sending.
        {
            USART_ClearFlag(DEF_UART, USART_FLAG_TC);
            USART_DMACmd(DEF_UART, USART_DMAReq_Tx, DISABLE);
            UART_Tx_Flag = 0;
            NVIC_DisableIRQ(USBFS_IRQn);                                                  // Disable USB interrupts
            RingBuffer_Comm.RemainPack--;
            RingBuffer_Comm.DealPtr++;
            if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
            {
                RingBuffer_Comm.DealPtr = 0;
            }
            NVIC_EnableIRQ(USBFS_IRQn);                                                   // Enable USB interrupts
        }
    }
    else
    {
        /* Pass-through USB-HID data to uart2 */
        if(RingBuffer_Comm.RemainPack)
        {
            pkg_len = Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE];      // Get the valid data length
            if (pkg_len)
            {
                if (pkg_len > ( DEF_USBD_FS_PACK_SIZE - 1 ) )
                {
                    pkg_len = DEF_USBD_FS_PACK_SIZE - 1;                                   // Limit the length of this transmission
                }
                pbuf = &Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE] + 1;
                UART_DMA_Tx( pbuf, pkg_len );
                UART_Tx_Flag = 1;
            }
            else
            {
                /* drop out */
                NVIC_DisableIRQ(USBFS_IRQn);                                                  // Disable USB interrupts
                RingBuffer_Comm.RemainPack--;
                RingBuffer_Comm.DealPtr++;
                if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                {
                    RingBuffer_Comm.DealPtr = 0;
                }
                NVIC_EnableIRQ(USBFS_IRQn);                                                   // Enable USB interrupts
            }
        }
    }

    /* Monitor whether the remaining space is available for further downloads */
    if(RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART))
    {
        if(RingBuffer_Comm.StopFlag)
        {
            printf("USB ring buffer full, stop receiving further data.\r\n");
            RingBuffer_Comm.StopFlag = 0;
            USBFSD->UEP1_RX_CTRL = (USBFSD->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
        }
    }

}

/*********************************************************************
 * @fn      HID_Set_Report_Deal
 *
 * @brief   print hid set report data
 *
 * @return  none
 */
void HID_Set_Report_Deal()
{
    uint16_t i;
    if (HID_Set_Report_Flag == SET_REPORT_WAIT_DEAL)
    {
        printf("Set Report:\r\n");
        for (i = 0; i < 64; ++i)
        {
            printf("%02x ",HID_Report_Buffer[i]);
        }
        printf("\r\n");
        HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;
        USBFSD->UEP0_TX_LEN  = 0;
        USBFSD->UEP0_TX_CTRL =  USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;

    }
}




