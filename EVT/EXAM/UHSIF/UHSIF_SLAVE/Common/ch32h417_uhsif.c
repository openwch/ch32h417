/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_uhsif.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/26
 * Description        : This file provides all the UHSIF firmware functions.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32H417_uhsif.h"
#include "ch32h417_usbss_device.h"

pack_t UHSIF_WR_Pack[ DEF_UHSIF_TXBUF_CNT ];
comm_t UHSIF_WR_COMM;
pack_t UHSIF_RD_Pack[ DEF_UHSIF_RXBUF_CNT ];
comm_t UHSIF_RD_COMM;

/*******************************************************************************
 * @fn      UHSIF_Init
 *
 * @brief   UHSIF initialization.
 *
 * @param   None
 *
 * @return  None
 */
 void UHSIF_Init( void )
 {
    UHSIF_Para_Init( );

    /* For the UHSIF pin initialization, the host needs to configure the frequency division coefficient */
    // UHSIF_GPIO_Init( DEF_UHSIF_SLAVE_FPGA_MODE, DEF_UHSIF_PINREMAP2, DEF_UHSIF_CLKREMAP1, 0, DEF_UHSIF_DATA_BIT32 );
    UHSIF_GPIO_Init( DEF_UHSIF_SLAVE_SOC_MODE, DEF_UHSIF_PINREMAP2, DEF_UHSIF_CLKREMAP1, 0 , DEF_UHSIF_DATA_BIT32 ); 
    // UHSIF_GPIO_Init( DEF_UHSIF_MASTER_MODE, DEF_UHSIF_PINREMAP2, DEF_UHSIF_CLKREMAP1, RCC_UHSIFDIV_DIV4, DEF_UHSIF_DATA_BIT32 );

    /* Initialization of UHSIF-related registers */  
    UHSIF_Cfg( );
    
    /* UHSIF line and buff initialization */  
    UHSIF_Line_Cfg( DEF_LINE0, DEF_LINE_DIR_IN, DEF_BUFF_COUNT_8, USBSS_EP1_Tx_Buf, 0 );
    UHSIF_Line_Cfg( DEF_LINE1, DEF_LINE_DIR_OUT, DEF_BUFF_COUNT_8, USBSS_EP1_Rx_Buf, 0 );

    // UHSIF_Line_Cfg( DEF_LINE0, DEF_LINE_DIR_IN, DEF_BUFF_COUNT_8, USBSS_EP1_Tx_Buf, 254 );
    // UHSIF_Line_Cfg( DEF_LINE1, DEF_LINE_DIR_OUT, DEF_BUFF_COUNT_8, USBSS_EP1_Rx_Buf, 5 );

    /* UHSIF enabled */  
    UHSIF_Start( ENABLE );
 }

/*******************************************************************************
 * @fn      UHSIF_Para_Init
 *
 * @brief   UHSIF-Related initialization.
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Para_Init( void )
{
    uint8_t i;

    UHSIF_WR_COMM.load = 0;
    UHSIF_WR_COMM.deal = 0;
    UHSIF_WR_COMM.total = 0;
    UHSIF_WR_COMM.stop = 0;

    UHSIF_RD_COMM.load = 0;
    UHSIF_RD_COMM.deal = 0;
    UHSIF_RD_COMM.total = 0;
    UHSIF_RD_COMM.stop = 0;

    for( i = 0; i < DEF_UHSIF_TXBUF_CNT; i++ )
    {
        UHSIF_WR_Pack[ i ].adr = (uint32_t)&USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * i ];
        UHSIF_WR_Pack[ i ].len = 0;
    }

    for( i = 0; i < DEF_UHSIF_RXBUF_CNT; i++ )
    {
        UHSIF_RD_Pack[ i ].adr = (uint32_t)&USBSS_EP1_Tx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * i ];
        UHSIF_RD_Pack[ i ].len = 0;
    }
}

/*******************************************************************************
 * @fn      UHSIF_Line0_IN_Callback
 *
 * @brief   The interrupt callback function for line 0 configuration input
 *
 * @param   rcv_count: The length of the data received by line 0
 *
 * @return  None
 */
void UHSIF_Line0_IN_Callback( uint32_t rcv_count )
{
    uint32_t cnt = DEF_TX_CHAIN_TIMEOUT;

    if( rcv_count )
    {
        UHSIF_RD_Pack[ UHSIF_RD_COMM.load ].len = rcv_count;

        NVIC_DisableIRQ(USBSS_IRQn);
        UHSIF_RD_COMM.total++;  
        NVIC_EnableIRQ(USBSS_IRQn);
    
        if( UHSIF_RD_COMM.total == 1 ) 
        {
            while( cnt-- )
            {
                if(( USBSSD->EP1_TX.UEP_TX_ST & USBSS_EP_TX_CHAIN_EN_MASK ) != USBSS_EP_TX_CHAIN_EN_MASK )
                {
                    if( UHSIF_RD_Pack[ UHSIF_RD_COMM.load ].len % 1024 )
                    {
                        USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = UHSIF_RD_Pack[ UHSIF_RD_COMM.load ].len % 1024;
                        USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = UHSIF_RD_Pack[ UHSIF_RD_COMM.load ].len / 1024 + 1;
                    }
                    else 
                    {
                        USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = 1024;
                        USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = UHSIF_RD_Pack[ UHSIF_RD_COMM.load ].len / 1024;
                    }

                    break;
                }
            }          
        }
        UHSIF_RD_COMM.load++;                                         
        if( UHSIF_RD_COMM.load >= DEF_UHSIF_RXBUF_CNT )                
        {
            UHSIF_RD_COMM.load = 0;
        }
    }

}

/*******************************************************************************
 * @fn      UHSIF_Line1_IN_Callback
 *
 * @brief   The interrupt callback function for line 1 configuration input
 *
 * @param   rcv_count: The length of the data received by line 1
 *
 * @return  None
 */
void UHSIF_Line1_IN_Callback( uint32_t rcv_count )
{
    
}

/*******************************************************************************
 * @fn      UHSIF_Line2_IN_Callback
 *
 * @brief   The interrupt callback function for line 2 configuration input
 *
 * @param   rcv_count: The length of the data received by line 2
 *
 * @return  None
 */
void UHSIF_Line2_IN_Callback( uint32_t rcv_count )
{
    
}

/*******************************************************************************
 * @fn      UHSIF_Line3_IN_Callback
 *
 * @brief   The interrupt callback function for line 3 configuration input
 *
 * @param   rcv_count: The length of the data received by line 3
 *
 * @return  None
 */
void UHSIF_Line3_IN_Callback( uint32_t rcv_count )
{
    
}

/*******************************************************************************
 * @fn      UHSIF_Line0_OUT_Callback
 *
 * @brief   The interrupt callback function for line 0 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line0_OUT_Callback( void )
{

}

/*******************************************************************************
 * @fn      UHSIF_Line1_OUT_Callback
 *
 * @brief   The interrupt callback function for line 1 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line1_OUT_Callback( void )
{

    UHSIF_WR_COMM.deal++;
    if( UHSIF_WR_COMM.deal >= DEF_UHSIF_TXBUF_CNT )               
    {
        UHSIF_WR_COMM.deal = 0;
    }

    if( UHSIF_WR_COMM.total )
    {
        NVIC_DisableIRQ(USBSS_IRQn);
        UHSIF_WR_COMM.total--;
        NVIC_EnableIRQ(USBSS_IRQn);
    }

    /* Monitor whether the remaining space is available for further downloads */
    if( UHSIF_WR_COMM.total < ( DEF_UHSIF_TXBUF_CNT - 1 ))
    {
        if( UHSIF_WR_COMM.stop )
        {
            UHSIF_WR_COMM.stop = 0;
            USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL;
        }
    }
}

/*******************************************************************************
 * @fn      UHSIF_Line2_OUT_Callback
 *
 * @brief   The interrupt callback function for line 2 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line2_OUT_Callback( void )
{
    
}

/*******************************************************************************
 * @fn      UHSIF_Line3_OUT_Callback
 *
 * @brief   The interrupt callback function for line 3 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line3_OUT_Callback( void )
{
    
}

 