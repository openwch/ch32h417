/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.01
* Date               : 2024/01/19
* Description        : uart serial port related initialization and processing
*******************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "UART.h"
#include "ch32h417_usb.h"
/*******************************************************************************/
/* Variable Definition */
/* Global */
void TIM2_IRQHandler( void )__attribute__((interrupt("WCH-Interrupt-fast")));


/* The following are serial port transmit and receive related variables and buffers */
volatile UART_CTL Uart;

__attribute__ ((aligned(4))) uint8_t  UART_Tx_Buf[ DEF_UARTx_TX_BUF_LEN ];  /* Serial port transmit data buffer */
__attribute__ ((aligned(4))) uint8_t  UART_Rx_Buf[ DEF_UARTx_RX_BUF_LEN ];  /* Serial port receive data buffer */
volatile uint32_t UARTx_Rx_DMACurCount;                       /* Serial port receive dma current counter */
volatile uint32_t UARTx_Rx_DMALastCount;                      /* Serial port receive dma last value counter  */

/*********************************************************************
 * @fn      RCC_Configuration
 *
 * @brief   Configures the different system clocks.
 *
 * @return  none
 */
uint8_t RCC_Configuration( void )
{
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOF | RCC_HB2Periph_GPIOA, ENABLE );
    RCC_HB1PeriphClockCmd( RCC_HB1Periph_USART4 | RCC_HB1Periph_TIM2, ENABLE );
    RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );
    return 0;
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   100us Timer
 *          144 * 100 * 13.8888 -----> 100uS
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    TIM_DeInit( TIM2 );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    /* Clear TIM2 update pending flag */
    TIM_ClearFlag( TIM2, TIM_FLAG_Update );

    /* TIM IT enable */
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

    /* Enable Interrupt */
    NVIC_EnableIRQ( TIM2_IRQn );

    /* TIM2 enable counter */
    TIM_Cmd( TIM2, ENABLE );
}

/*********************************************************************
 * @fn      UART_CfgInit
 *
 * @brief   Uart configuration initialization
 *
 * @return  none
 */
void UART_CfgInit( uint32_t baudrate, uint8_t stopbits, uint8_t parity )
{
    USART_InitTypeDef USART_InitStructure = {0};
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    uint16_t dat = dat;

    /* delete contains in ( ... )  */
    /* First set the serial port introduction to output high then close the TE and RE of CTLR1 register (note that USARTx->CTLR1 register setting 9 bits has a limit) */
    /* Note: This operation must be performed, the TX pin otherwise the level will be pulled low */
    GPIO_SetBits( GPIOF, GPIO_Pin_4 );
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOF, &GPIO_InitStructure );

    /* clear te/re */
    DEF_UART->CTLR1 &= ~( USART_CTLR1_TE | USART_CTLR1_RE );

    /* USART Hard configured: */
    /* Configure USART Rx (PF3) as input floating */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init( GPIOF, &GPIO_InitStructure );
    GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF7);

    /* Configure USART Tx (PF4) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOF, &GPIO_InitStructure );
    GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF7);
    /* Test IO */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
    */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    /* Number of stop bits (0: 1 stop bit; 1: 1.5 stop bits; 2: 2 stop bits). */
    if( stopbits == 1 )
    {
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
    }
    else if( stopbits == 2 )
    {
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    }
    else
    {
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    }

    /* Check digit (0: None; 1: Odd; 2: Even; 3: Mark; 4: Space); */
    if( parity == 1 )
    {
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    }
    else if( parity == 2 )
    {
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    }
    else
    {
        USART_InitStructure.USART_Parity = USART_Parity_No;
    }
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( DEF_UART, &USART_InitStructure );
    USART_ClearFlag( DEF_UART, USART_FLAG_TC );

    /* Enable USART */
    USART_Cmd( DEF_UART, ENABLE );
}

/*********************************************************************
 * @fn      UART_ParaInit
 *
 * @brief   CDC2 parameters initialization
 *          mode = 0 : Used in usb modify initialization
 *          mode = 1 : Used in default initializations
 * @return  none
 */
void UART_ParaInit( uint8_t mode )
{
    uint8_t i;

    Uart.Rx_LoadPtr = 0x00;
    Uart.Rx_DealPtr = 0x00;
    Uart.Rx_RemainLen = 0x00;
    Uart.Rx_TimeOut = 0x00;
    Uart.Rx_TimeOutMax = 30;

    Uart.Tx_LoadNum = 0x00;
    Uart.Tx_DealNum = 0x00;
    Uart.Tx_RemainNum = 0x00;
    for( i = 0; i < DEF_UARTx_TX_BUF_NUM_MAX; i++ )
    {
        Uart.Tx_PackLen[ i ] = 0x00;
    }
    Uart.Tx_Flag = 0x00;
    Uart.Tx_CurPackLen = 0x00;
    Uart.Tx_CurPackPtr = 0x00;

    Uart.USB_Up_IngFlag = 0x00;
    Uart.USB_Up_TimeOut = 0x00;
    Uart.USB_Up_Pack0_Flag = 0x00;
    Uart.USB_Down_StopFlag = 0x00;
    UARTx_Rx_DMACurCount = 0x00;
    UARTx_Rx_DMALastCount = 0x00;

    if( mode )
    {
        Uart.Com_Cfg[ 0 ] = (uint8_t)( DEF_UARTx_BAUDRATE );
        Uart.Com_Cfg[ 1 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 8 );
        Uart.Com_Cfg[ 2 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 16 );
        Uart.Com_Cfg[ 3 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 24 );
        Uart.Com_Cfg[ 4 ] = DEF_UARTx_STOPBIT;
        Uart.Com_Cfg[ 5 ] = DEF_UARTx_PARITY;
        Uart.Com_Cfg[ 6 ] = DEF_UARTx_DATABIT;
        Uart.Com_Cfg[ 7 ] = DEF_UARTx_RX_TIMEOUT;
    }
}


/*********************************************************************
 * @fn      UART_DMAInit
 *
 * @brief   Uart DMA configuration initialization
 *          type = 0 : USART_TX
 *          type = 1 : USART_RX
 *          pbuf     : Tx/Rx Buffer, should be aligned(4)
 *          len      : buffer size of Tx/Rx Buffer
 *
 * @return  none
 */
void UART_DMAInit( uint8_t type, uint8_t *pbuf, uint32_t len )
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    if( type == 0x00 )
    {
        /* UART Tx-DMA configuration */
        DMA_DeInit( DEF_UART_TX_DMA_CH );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&DEF_UART->DATAR);
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DEF_UART_TX_DMA_CH, &DMA_InitStructure );
        DMA_MuxChannelConfig(DEF_UART_TX_MUX_CH, DEF_UART_TX_DMA_REQ);
        DMA_Cmd( DEF_UART_TX_DMA_CH, ENABLE );
    }
    else
    {
        /* UART Rx-DMA configuration */
        DMA_DeInit( DEF_UART_RX_DMA_CH );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&DEF_UART->DATAR);
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DEF_UART_RX_DMA_CH, &DMA_InitStructure );
        DMA_MuxChannelConfig(DEF_UART_RX_MUX_CH, DEF_UART_RX_DMA_REQ);
        DMA_Cmd( DEF_UART_RX_DMA_CH, ENABLE );
    }
}

/*********************************************************************
 * @fn      UART_Init
 *
 * @brief   CDC total initialization
 *          mode     : See the useage of UART_ParaInit( mode )
 *          baudrate : Serial port 2 default baud rate
 *          stopbits : Serial port 2 default stop bits
 *          parity   : Serial port 2 default parity
 *
 * @return  none
 */
void UART_Init( uint8_t mode, uint32_t baudrate, uint8_t stopbits, uint8_t parity )
{
    USART_DMACmd( DEF_UART, USART_DMAReq_Rx, DISABLE );
    DMA_Cmd( DEF_UART_TX_DMA_CH, DISABLE );
    DMA_Cmd( DEF_UART_RX_DMA_CH, DISABLE );

    UART_CfgInit( baudrate, stopbits, parity );
    UART_DMAInit( 0, &UART_Tx_Buf[ 0 ], 0 );
    UART_DMAInit( 1, &UART_Rx_Buf[ 0 ], DEF_UARTx_RX_BUF_LEN );

    USART_DMACmd( DEF_UART, USART_DMAReq_Rx, ENABLE );

    UART_ParaInit( mode );
}

/*********************************************************************
 * @fn      UART_USB_Init
 *
 * @brief   CDC2 initialization in usb interrupt
 *
 * @return  none
 */
void UART_USB_Init( void )
{
    uint32_t baudrate;
    uint8_t  stopbits;
    uint8_t  parity;

    baudrate = ( uint32_t )( Uart.Com_Cfg[ 3 ] << 24 ) + ( uint32_t )( Uart.Com_Cfg[ 2 ] << 16 );
    baudrate += ( uint32_t )( Uart.Com_Cfg[ 1 ] << 8 ) + ( uint32_t )( Uart.Com_Cfg[ 0 ] );
    stopbits = Uart.Com_Cfg[ 4 ];
    parity = Uart.Com_Cfg[ 5 ];

    UART_Init( 0, baudrate, stopbits, parity );

    /* restart usb receive  */
    USBFSD->UEP2_DMA = (uint32_t)(uint8_t *)&UART_Tx_Buf[ 0 ];
    USBFSD->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
    USBFSD->UEP2_RX_CTRL |= USBFS_UEP_R_RES_ACK;
}

/*********************************************************************
 * @fn      UART_DataTx_Deal
 *
 * @brief   CDC2 data transmission processing
 *
 * @return  none
 */
void UART_DataTx_Deal( void )
{
    uint16_t  count;

    /* uart transmission processing */
    if( Uart.Tx_Flag )
    {
        /* Query whether the DMA transmission of the serial port is completed */
        if( DEF_UART->STATR & USART_FLAG_TC )
        {
            DEF_UART->STATR = (uint16_t)( ~USART_FLAG_TC );
            DEF_UART->CTLR3 &= ( ~USART_DMAReq_Tx );

            Uart.Tx_Flag = 0x00;

            NVIC_DisableIRQ( USBFS_IRQn );
            NVIC_DisableIRQ( USBFS_IRQn );

            /* Calculate the variables of last data */
            count = Uart.Tx_CurPackLen - DEF_UART_TX_DMA_CH->CNTR;
            Uart.Tx_CurPackLen -= count;
            Uart.Tx_CurPackPtr += count;
            if( Uart.Tx_CurPackLen == 0x00 )
            {
                Uart.Tx_PackLen[ Uart.Tx_DealNum ] = 0x0000;
                Uart.Tx_DealNum++;
                if( Uart.Tx_DealNum >= DEF_UARTx_TX_BUF_NUM_MAX )
                {
                    Uart.Tx_DealNum = 0x00;
                }
                Uart.Tx_RemainNum--;
            }

            /* If the current serial port has suspended the downlink, restart the driver downlink */
            if( ( Uart.USB_Down_StopFlag == 0x01 ) && ( Uart.Tx_RemainNum < 2 ) )
            {
                USBFSD->UEP2_RX_CTRL = (USBFSD->UEP2_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
                Uart.USB_Down_StopFlag = 0x00;
            }

            NVIC_EnableIRQ( USBFS_IRQn );
        }
    }
    else
    {
        /* Load data from the serial port send buffer to send  */
        if( Uart.Tx_RemainNum )
        {
            
            /* Determine whether to load from the last unsent buffer or from a new buffer */
            if( Uart.Tx_CurPackLen == 0x00 )
            {
                Uart.Tx_CurPackLen = Uart.Tx_PackLen[ Uart.Tx_DealNum ];
                Uart.Tx_CurPackPtr = ( Uart.Tx_DealNum * DEF_USB_FS_PACK_LEN );
            }
            /* Configure DMA and send */
            USART_ClearFlag( DEF_UART, USART_FLAG_TC );
            DMA_Cmd( DEF_UART_TX_DMA_CH, DISABLE );
            DEF_UART_TX_DMA_CH->MADDR = (uint32_t)&UART_Tx_Buf[ Uart.Tx_CurPackPtr ];
            DEF_UART_TX_DMA_CH->CNTR = Uart.Tx_CurPackLen;
            DMA_Cmd( DEF_UART_TX_DMA_CH, ENABLE );
            DEF_UART->CTLR3 |= USART_DMAReq_Tx;
            Uart.Tx_Flag = 0x01;
        }
    }
}

/*********************************************************************
 * @fn      UART_DataRx_Deal
 *
 * @brief   Uart data receiving processing
 *
 * @return  none
 */
void UART_DataRx_Deal( void )
{
    uint16_t temp16;
    uint32_t remain_len;
    uint16_t packlen;

    /* Serial port data DMA receive processing */
    NVIC_DisableIRQ( USBFS_IRQn );
    NVIC_DisableIRQ( USBFS_IRQn );
    UARTx_Rx_DMACurCount = DEF_UART_RX_DMA_CH->CNTR;
    if( UARTx_Rx_DMALastCount != UARTx_Rx_DMACurCount )

    {
        if( UARTx_Rx_DMALastCount > UARTx_Rx_DMACurCount )
        {
            temp16 = UARTx_Rx_DMALastCount - UARTx_Rx_DMACurCount;
        }
        else
        {
            temp16 = DEF_UARTx_RX_BUF_LEN - UARTx_Rx_DMACurCount;
            temp16 += UARTx_Rx_DMALastCount;
        }
        UARTx_Rx_DMALastCount = UARTx_Rx_DMACurCount;
        if( ( Uart.Rx_RemainLen + temp16 ) > DEF_UARTx_RX_BUF_LEN )
        {
            /* Overflow handling */
            /* Save frame error status */
            printf("U0_O:%08lx\n",(uint32_t)Uart.Rx_RemainLen);
        }
        else
        {
            Uart.Rx_RemainLen += temp16;
        }

        /* Setting reception status */
        Uart.Rx_TimeOut = 0x00;
    }
    NVIC_EnableIRQ( USBFS_IRQn );

    /*****************************************************************/
    /* Serial port data processing via USB upload and reception */
    if( Uart.Rx_RemainLen )
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            /* Calculate the length of this upload */
            remain_len = Uart.Rx_RemainLen;
            packlen = 0x00;
            if( remain_len >= DEF_USBD_FS_PACK_SIZE )
            {
                packlen = DEF_USBD_FS_PACK_SIZE;
            }
            else
            {
                if( Uart.Rx_TimeOut >= Uart.Rx_TimeOutMax )
                {
                    packlen = remain_len;
                }
            }
            if( packlen > ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr ) )
            {
                packlen = ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr );
            }

            /* Upload serial data via usb */
            if( packlen )
            {
                NVIC_DisableIRQ( USBFS_IRQn );
                NVIC_DisableIRQ( USBFS_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;

                memcpy(USBFS_EP3_Buf,&UART_Rx_Buf[ Uart.Rx_DealPtr ],packlen);
                USBFSD->UEP3_TX_LEN = packlen;
                USBFSD->UEP3_TX_CTRL = (USBFSD->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
				NVIC_EnableIRQ( USBFS_IRQn );

                /* Calculate the variables of interest */
                Uart.Rx_RemainLen -= packlen;
                Uart.Rx_DealPtr += packlen;
                if( Uart.Rx_DealPtr >= DEF_UARTx_RX_BUF_LEN )
                {
                    Uart.Rx_DealPtr = 0x00;
                }

                /* Start 0-length packet timeout timer */
                if( packlen == DEF_USBD_FS_PACK_SIZE )
                {
                    Uart.USB_Up_Pack0_Flag = 0x01;
                }
            }
        }
        else
        {
            /* Set the upload success flag directly if the upload is not successful after the timeout */
            if( Uart.USB_Up_TimeOut >= DEF_UARTx_USB_UP_TIMEOUT )
            {
                Uart.USB_Up_IngFlag = 0x00;
            }
        }
    }

    /*****************************************************************/
    /* Determine if a 0-length packet needs to be uploaded (required for CDC mode) */
    if( Uart.USB_Up_Pack0_Flag )
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            if( Uart.USB_Up_TimeOut >= ( DEF_UARTx_RX_TIMEOUT * 20 ) )
            {
                NVIC_DisableIRQ( USBFS_IRQn );
                NVIC_DisableIRQ( USBFS_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;
                USBFSD->UEP3_TX_LEN = 0;
                USBFSD->UEP3_TX_CTRL = (USBFSD->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
                Uart.USB_Up_IngFlag = 0;
                Uart.USB_Up_Pack0_Flag = 0x00;
                NVIC_EnableIRQ( USBFS_IRQn );
            }
        }
    }
}


/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
void TIM2_IRQHandler( void )
{
    /* Test IO */
    static uint8_t tog;
    tog ? (GPIOA->BSHR = GPIO_Pin_15):(GPIOA->BCR = GPIO_Pin_15);
    tog ^= 1;
    /* uart timeout counts */
    Uart.Rx_TimeOut++;
    Uart.USB_Up_TimeOut++;

    /* clear status */
    TIM2->INTFR = (uint16_t)~TIM_IT_Update;
}